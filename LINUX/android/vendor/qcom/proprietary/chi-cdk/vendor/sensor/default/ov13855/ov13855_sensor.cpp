////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camxsensordriverapi.h"

// NOWHINE ENTIRE FILE

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/* CONSTANTS */
#define OV13855_MIN_AGAIN_REG_VAL       128   /* 1.0x */
#define OV13855_MAX_AGAIN_REG_VAL       1984  /* 15.5x */

#define OV13855_AGAIN_DECIMATOR         128

#define OV13855_MIN_DGAIN_REG_VAL       1024  /* 1.0x */
#define OV13855_MAX_DGAIN_REG_VAL       1024  /* 1.0x */

#define OV13855_MAX_DGAIN_DECIMATOR     1024

/* OV13855 FORMULAS */
#define OV13855_MIN_AGAIN               1.0
#define OV13855_MAX_AGAIN               15.5

#define OV13855_MIN_DGAIN               (OV13855_MIN_DGAIN_REG_VAL / 1024)
#define OV13855_MAX_DGAIN               (OV13855_MAX_DGAIN_REG_VAL / 1024)

#define OV13855_MIN_GAIN                (OV13855_MIN_AGAIN*OV13855_MIN_DGAIN)
#define OV13855_MAX_GAIN                (OV13855_MAX_AGAIN*OV13855_MAX_DGAIN)


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Analog Gain To Register Val
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static UINT AnalogGainToRegisterVal(
    FLOAT aGain)
{
    UINT regVal = 0;

    if (aGain < (FLOAT)OV13855_MIN_AGAIN)
    {
        aGain = (FLOAT)OV13855_MIN_AGAIN;
    }
    else if (aGain > (FLOAT)OV13855_MAX_AGAIN)
    {
        aGain = (FLOAT)OV13855_MAX_AGAIN;
    }

    regVal = (UINT)(aGain * OV13855_AGAIN_DECIMATOR);
    if (regVal < 0x200)
    {
        regVal = (regVal >> 4 ) << 4;
    }
    else if (regVal < 0x400)
    {
        regVal = (regVal >> 5 ) << 5;
    }
    else if (regVal < 0x800)
    {
        regVal = (regVal >> 6 ) << 6;
    }

    return regVal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Register Val To Analog Gain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static FLOAT RegisterValToAnalogGain(
    UINT regVal)
{
    FLOAT aGain = 0.0;

    if (regVal < OV13855_MIN_AGAIN_REG_VAL)
    {
        regVal = OV13855_MIN_AGAIN_REG_VAL;
    }
    else if(regVal > OV13855_MAX_AGAIN_REG_VAL)
    {
        regVal = OV13855_MAX_AGAIN_REG_VAL;
    }
    aGain = (float)regVal / OV13855_AGAIN_DECIMATOR;

    return aGain;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calcuate the sensor digital gain.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static unsigned int CalculateDigitalGainRegesterVal(float realGain, float aGain)
{
    UINT    regVal  = OV13855_MIN_DGAIN_REG_VAL;
    FLOAT   dGain   = OV13855_MIN_DGAIN;

    if(realGain > OV13855_MAX_AGAIN)
    {
        dGain = realGain / aGain;
    }
    else
    {
        dGain = OV13855_MIN_DGAIN;
    }

    if(dGain > OV13855_MAX_DGAIN_REG_VAL)
    {
        dGain = OV13855_MAX_DGAIN_REG_VAL;
    }
    regVal = (UINT)(dGain * 1024);

    return regVal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FillExposureSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CalculateExposure(
    SensorExposureInfo*          pExposureInfo,
    SensorCalculateExposureData* pCalculateExposureData)
{
    if (NULL == pExposureInfo || NULL == pCalculateExposureData)
    {
        return FALSE;
    }

    pExposureInfo->analogRegisterGain   = AnalogGainToRegisterVal(pCalculateExposureData->realGain);
    pExposureInfo->analogRealGain       = static_cast<FLOAT>(RegisterValToAnalogGain(pExposureInfo->analogRegisterGain));
    pExposureInfo->digitalRegisterGain  =
        CalculateDigitalGainRegesterVal(pCalculateExposureData->realGain, pExposureInfo->analogRealGain);
    pExposureInfo->digitalRealGain      = (FLOAT)pExposureInfo->digitalRegisterGain / OV13855_MAX_DGAIN_DECIMATOR;
    pExposureInfo->ISPDigitalGain       =
        pCalculateExposureData->realGain / pExposureInfo->analogRealGain / pExposureInfo->digitalRealGain;
    pExposureInfo->lineCount            = pCalculateExposureData->lineCount;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FillExposureSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL FillExposureSettings(
    RegSettingsInfo*        pRegSettingsInfo,
    SensorFillExposureData* pExposureData)
{
    UINT32  index     = 0;
    UINT    regCount  = 0;

    if ((NULL == pRegSettingsInfo) || (NULL == pExposureData))
    {
        return FALSE;
    }

    for (index = 0; index < pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount; index++)
    {
        pRegSettingsInfo->regSetting[regCount].registerAddr =
            pExposureData->pRegInfo->groupHoldOnSettings.regSetting[index].registerAddr;
        pRegSettingsInfo->regSetting[regCount].registerData =
            pExposureData->pRegInfo->groupHoldOnSettings.regSetting[index].registerData;
        pRegSettingsInfo->regSetting[regCount].regAddrType  =
            pExposureData->pRegInfo->groupHoldOnSettings.regSetting[index].regAddrType;
        pRegSettingsInfo->regSetting[regCount].regDataType  =
            pExposureData->pRegInfo->groupHoldOnSettings.regSetting[index].regDataType;
        pRegSettingsInfo->regSetting[regCount].delayUs      =
            pExposureData->pRegInfo->groupHoldOnSettings.regSetting[index].delayUs;
        regCount++;
    }

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->frameLengthLinesAddr;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->frameLengthLines & 0xFF00) >> 8;
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->frameLengthLinesAddr + 1;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->frameLengthLines & 0xFF);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->coarseIntgTimeAddr;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->lineCount >> 12) & 0xFF;
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->coarseIntgTimeAddr + 1;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->lineCount >> 4) & 0xFF;
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->coarseIntgTimeAddr + 2;
    pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->lineCount << 4) & 0xFF;
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->globalAnalogGainAddr;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->analogRegisterGain & 0xFF00) >> 8;
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->globalAnalogGainAddr+ 1;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->analogRegisterGain & 0xFF);
    regCount++;

    for (index = 0; (pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount + index) < regCount; index++)
    {
        pRegSettingsInfo->regSetting[pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount + index].regAddrType  =
            I2CRegAddressDataTypeWord;
        pRegSettingsInfo->regSetting[pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount + index].regDataType  =
            I2CRegAddressDataTypeByte;
        pRegSettingsInfo->regSetting[pExposureData->pRegInfo->groupHoldOnSettings.regSettingCount + index].delayUs      =
            0;
    }

    for (index = 0; index < pExposureData->pRegInfo->groupHoldOffSettings.regSettingCount; index++)
    {
        pRegSettingsInfo->regSetting[regCount].registerAddr  =
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].registerAddr;
        pRegSettingsInfo->regSetting[regCount].registerData  =
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].registerData;
        pRegSettingsInfo->regSetting[regCount].regAddrType  =
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].regAddrType;
        pRegSettingsInfo->regSetting[regCount].regDataType=
            pExposureData->pRegInfo->groupHoldOffSettings.regSetting[index].regDataType;
        regCount++;
    }

    pRegSettingsInfo->regSettingCount = regCount;

    if (MAX_REG_SETTINGS <= regCount)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
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
    pSensorLibraryAPI->pFillExposureSettings = FillExposureSettings;
}

#ifdef __cplusplus
} // CamX Namespace
#endif // __cplusplus
