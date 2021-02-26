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

$Header: //components/rel/core.qdsp6/2.1.c4/settings/power/qdsp_pm/config/670/cdsp/qdsp_pm_configdata_arch_670_cdsp.h#1 $
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
 * Table of DDR compensation factors to be applied at different BW ranges
 * This is used to compensate for bus latencies/cache misses
 * Latency/Snoc floor vote also updated
 */
const AsicCompensatedDdrBwTableEntryType compensatedDdrBw_array_670[] = {
        //        bwThreshold,             Q6Ab,                  Q6Ib,          Latency Vote,      snocfloorVote      honestflag
        {             5000000,          5000000,             152000000,          0,                  0,                 0,},
        {            40000000,                0,             592000000,          0,                  0,                 1,},
        {            75000000,                0,             592000000,          0,                  0,                 1,},
        {           100000000,                0,             792000000,          0,                  0,                 1,},
        {           160000000,                0,            1592000000,          0,                  0,                 1,},
        {           200000000,                0,            1592000000,          0,                  0,                 1,},
        {          ULLONG_MAX,                0,            3192000000,          0,                  0,                 1,}
};

AsicCompensatedDdrBwTableType compensatedDdrBwTable_670 = {ADSPPM_ARRAY(compensatedDdrBw_array_670)};

//todo: correct values from audio arch team
const AsicSnocVoteTableEntryType adspsnocBwVote_array_670[] = {
        //        bwThreshold,            bwAbVote,      bwIbVote,    latencyVote,  Fal_toleerance,     Idle_time,      honestFlag
        {             5000000,             5000000,      76000000,              0,           12500,         10000,            0,},
        {            40000000,                   0,     840000000,              0,            2100,         10000,            1,},
        {            75000000,                   0,     840000000,           1100,            1100,         10000,            1,},
        {           100000000,                   0,    1800000000,            900,             900,         10000,            1,},
        {           160000000,                   0,    2180000000,            600,             600,         10000,            1,},
        {           200000000,                   0,    4064000000,            500,             500,         10000,            1,},
        {          ULLONG_MAX,                   0,    5180000000,            300,             300,         10000,            1,}
};

AsicSnocVoteTableType adspsnocVoteTable_670 = {ADSPPM_ARRAY(adspsnocBwVote_array_670)};

// BW Compensation table for AHB_IX
const AsicCompensatedAhbBwTableEntryType compensatedAhbBw_array_670[] = {
    // bwThreshold,            bwVote
    {      9784305,          61440000 }, // target SVS2  19.2 Mhz
    {     19568611,         122880000 }, // target SVS2  38.4 Mhz
    {     39137222,         245760000 }, // target SVS   76.8 Mhz
    {    490240000,         490240000 }, // target NOM  153.2 Mhz
    {   ULLONG_MAX,                -1 },
};

AsicCompensatedAhbBwTableType compensatedAhbBwTable_670 = {ADSPPM_ARRAY(compensatedAhbBw_array_670)};

// BW Compensation table for AHBE/Noc
// Table doesn't have any entries since AHBE compensation is not applied on CDSP (Scaled only wrt Q6)
const AsicCompensatedAhbBwTableEntryType compensatedNocBw_array_670[] = {
    // bwThreshold,            bwVote
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

// Table for cache partition configuration.
// Audio and voice classes are grouped together for the purposes
// of determining class concurrency.
// Client class combinations not present in the table will default
// to a shared cache (no partitioning).
const AsicCachePartitionConfigType cachePartitionConfig_array_670[] = {

    // aggregateClass,                mainPartSize

    { MMPM_AUDIO_CLIENT_CLASS |
      MMPM_COMPUTE_CLIENT_CLASS,      FULL_SIZE }, // 2 way AV/comp

    { MMPM_VOICE_CLIENT_CLASS |
      MMPM_COMPUTE_CLIENT_CLASS,      FULL_SIZE }, // 2 way AV/comp

    { MMPM_AUDIO_CLIENT_CLASS |
      MMPM_VOICE_CLIENT_CLASS |
      MMPM_COMPUTE_CLIENT_CLASS,      FULL_SIZE }, // 2 way AV/comp

};

AsicCachePartitionConfigTableType cachePartitionConfigTable_670 =
    { ADSPPM_ARRAY(cachePartitionConfig_array_670) };

/**
Table for system cache configuration mapping
SC driver takes usecaseid as input to provide SCID
First column represents Client class; Second priority ID and third corrosponding usecaseId

For CDSP There are 2 Use-Cases:
SC_USE_CASE_ID_COMPUTE is used for voting from Q6 -> DDR
SC_USE_CASE_ID_COMPUTE_DMA is used to vote for VAPSS_DMA -> DDR

Since we're not controll the programming of COMPUTE_DMA use
case, we don't have it in our config table. However, we do
maintain the reference count for this SCID, and do
activation/deactivation sequence when triggered by DMA Driver.
*/

const AsicSystemCacheConfigType systemcacheConfig_array_670[] = {

    // clientClass,                 UsecaseId
    { MMPM_COMPUTE_CLIENT_CLASS,    SC_USE_CASE_ID_COMPUTE },
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
    {   288000000,       25600000 }, // MinSVS f-max
    {   460800000,       51200000 }, // lowSVS f-max
    {   614400000,      102400000 }, // svs f-max
    {   787200000,      122880000 }, // svs_l1 f-max                                     //
    {   940800000,      204800000 }, // nominal ADSP f-max
    {  1190400000,      245760000 }, // turbo
};

AsicAdspToAhbFreqTableType adspToAhbeFreqTable_670v1 =
    { ADSPPM_ARRAY(adspToAhbeFreq_array_670v1) };


const AsicAdspToAhbFreqTableEntry adspToAhbIxFreq_array_670v1[] = {
    // adspFreqHz,      abhIxFreqHz
    {   288000000,       0 }, // MinSVS f-max
    {   460800000,       0 }, // lowSVS f-max
    {   614400000,       0 }, // svs f-max
    {   787200000,       0 }, // svs_L1 f-max
    {   940800000,       153600000 }, // nominal ADSP f-max
    {   1190400000,      153600000 }, // turbo
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
    {   FULL_SIZE,            100  },
    {   THREE_QUARTER_SIZE,   100  },
    {   HALF_SIZE,            100  },
};

AsicAdspCacheSizeBWScalingTableType  adspcachesizebwscaling_670 = {ADSPPM_ARRAY(adspcachesizebwscaling_array_670)};

#endif

