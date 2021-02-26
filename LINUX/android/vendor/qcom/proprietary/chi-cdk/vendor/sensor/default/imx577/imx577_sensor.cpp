////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camxsensordriverapi.h"
// NOWHINE ENTIRE FILE
#include <android/log.h>
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define IMX577_DBG(fmt, args...) __android_log_print(ANDROID_LOG_INFO, "IMX577",fmt, ##args)

static const UINT   IMX577MinRegisterGain        = 0;                                     ///< Minimum analog register gain
static const UINT   IMX577MaxRegisterGain        = 978;                                   ///< Maximum analog register gain
static const DOUBLE IMX577MinRealGain            = (1024.0f / (1024.0f - IMX577MinRegisterGain));    ///< Minimum analog real gain (1X)
static const DOUBLE IMX577MaxRealGain            = (1024.0f / (1024.0f - IMX577MaxRegisterGain));    ///< Maximum analog real gain (16X)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RegisterToRealGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static DOUBLE RegisterToRealGain(
    UINT registerGain)
{

    if(IMX577MaxRegisterGain < registerGain)
    {
        registerGain = IMX577MaxRegisterGain;
    }

    return (1024.0f / (1024.0f - registerGain));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RealToRegisterGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static UINT RealToRegisterGain(
    DOUBLE realGain)
{
    UINT32 regGain = 0;

    if (realGain < 1.0)
    {
        realGain = 1.0;
    }
    else if (realGain > IMX577MaxRealGain)
    {
        realGain = IMX577MaxRealGain;
    }

    regGain = static_cast<UINT32>(1024.0 - (1024.0 / realGain));
    return regGain;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CalculateExposure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CalculateExposure(
    SensorExposureInfo*          pExposureInfo,
    SensorCalculateExposureData* pCalculateExposureData)
{
    if (NULL == pExposureInfo || NULL == pCalculateExposureData)
    {
        return FALSE;
    }

    pExposureInfo->analogRegisterGain   = RealToRegisterGain(pCalculateExposureData->realGain);
    pExposureInfo->analogRealGain       = static_cast<FLOAT>(RegisterToRealGain(pExposureInfo->analogRegisterGain));
    pExposureInfo->digitalRegisterGain  = 1;
    pExposureInfo->digitalRealGain      = 1;
    pExposureInfo->ISPDigitalGain       =
        pCalculateExposureData->realGain /(pExposureInfo->analogRealGain * pExposureInfo->digitalRealGain);
    pExposureInfo->lineCount            = pCalculateExposureData->lineCount;
    pExposureInfo->shortRegisterGain    = RealToRegisterGain(pCalculateExposureData->shortRealGain);
    pExposureInfo->shortLinecount       = pCalculateExposureData->shortLinecount;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetSensorLibraryAPIs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID GetSensorLibraryAPIs(
    SensorLibraryAPI* pSensorLibraryAPI)
{
    pSensorLibraryAPI->majorVersion          = 1;
    pSensorLibraryAPI->minorVersion          = 0;
    pSensorLibraryAPI->pCalculateExposure    = CalculateExposure;
}

#ifdef __cplusplus
} // CamX Namespace
#endif // __cplusplus
