// NOWHINE NC009 <- sensor special shared code
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  imx576_sensor.cpp
/// @brief imx576 exposure control implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camxsensordriverapi.h"
#include "chistatsinterfacedefs.h"
#include <android/log.h>
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define IMX576_DBG(fmt, args...) __android_log_print(ANDROID_LOG_INFO, "IMX576",fmt, ##args)
static const UINT   IMX576MinRegisterGain        = 0;                                     ///< Minimum analog register gain
static const UINT   IMX576MaxRegisterGain        = 960;                                   ///< Maximum analog register gain
static const DOUBLE IMX576MinRealGain            = (1024 / (1024 - IMX576MinRegisterGain)); ///< Minimum analog real gain (1X)
static const DOUBLE IMX576MaxRealGain            = (1024 / (1024 - IMX576MaxRegisterGain)); ///< Maximum analog real gain (16X)
static const UINT   IMX576MinDigitalRegisterGain = 256;                                   ///< Minimum digital register gain
static const UINT   IMX576MaxDigitalRegisterGain = 256;                                   ///< Maximum digital register gain
static const UINT   IMX576DigitalGainDecimator   = 256;                                   /// < Digital gain decimator factor
static const DOUBLE IMX576MinDigitalRealGain     = (IMX576MinDigitalRegisterGain / IMX576DigitalGainDecimator);
                                                                                          ///< Minimum digital real gain (1X)
static const DOUBLE IMX576MaxDigitalRealGain     = (IMX576MaxDigitalRegisterGain/ IMX576DigitalGainDecimator);
                                                                                          ///< Maximum digital real gain (1X)

static const UINT   IMX576HDRMaxDigitalRegisterGain = 4096;                              ///< HDR Maximum digital register gain
static const DOUBLE IMX576HDRMaxDigitalRealGain     = (IMX576HDRMaxDigitalRegisterGain / IMX576DigitalGainDecimator);
                                                                                         ///< HDR Maximum digital real gain(16X)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RegisterToRealGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static DOUBLE RegisterToRealGain(
    UINT registerGain)
{
    if (IMX576MaxRegisterGain < registerGain)
    {
        registerGain = IMX576MaxRegisterGain;
    }

    return  (1024.0 / (1024.0 - registerGain));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RealToRegisterGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static UINT RealToRegisterGain(
    DOUBLE realGain)
{
    if (IMX576MinRealGain > realGain)
    {
        realGain = IMX576MinRealGain;
    }
    else if (IMX576MaxRealGain < realGain)
    {
        realGain = IMX576MaxRealGain;
    }

    return static_cast<UINT>(1024.0 - (1024.0 / realGain));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CalculateDigitalGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static UINT CalculateDigitalGain(
    FLOAT totalRealGain,
    FLOAT sensorRealGain,
    BOOL  HDREnabled)
{
    float digitalRealGain = IMX576MinDigitalRealGain;

    if (IMX576MaxRealGain < totalRealGain)
    {
        digitalRealGain = totalRealGain / sensorRealGain;
    }
    else
    {
        digitalRealGain = IMX576MinDigitalRealGain;
    }

    if (TRUE == HDREnabled)
    {
        if (IMX576HDRMaxDigitalRealGain < digitalRealGain)
        {
            digitalRealGain = IMX576HDRMaxDigitalRealGain;
        }
    }
    else
    {
        if (IMX576MaxDigitalRealGain < digitalRealGain)
        {
            digitalRealGain = IMX576MaxDigitalRealGain;
        }
    }

    return static_cast<UINT>(digitalRealGain * IMX576DigitalGainDecimator);
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

    if ((pCalculateExposureData->middleRealGain == 0.0) || (pCalculateExposureData->middleLinecount == 0))
    {
        pExposureInfo->analogRegisterGain   = RealToRegisterGain(pCalculateExposureData->realGain);
        pExposureInfo->analogRealGain       = static_cast<FLOAT>(RegisterToRealGain(pExposureInfo->analogRegisterGain));
        pExposureInfo->digitalRegisterGain  =
            CalculateDigitalGain(pCalculateExposureData->realGain, pExposureInfo->analogRealGain, FALSE);
        pExposureInfo->digitalRealGain      =
            static_cast<FLOAT>(pExposureInfo->digitalRegisterGain / IMX576DigitalGainDecimator);
        pExposureInfo->ISPDigitalGain       =
            pCalculateExposureData->realGain /(pExposureInfo->analogRealGain * pExposureInfo->digitalRealGain);
        pExposureInfo->lineCount            = pCalculateExposureData->lineCount;
        pExposureInfo->shortRegisterGain    = RealToRegisterGain(pCalculateExposureData->shortRealGain);
        pExposureInfo->shortLinecount       = pCalculateExposureData->shortLinecount;
    }
    else
    {
        pExposureInfo->analogRegisterGain         = RealToRegisterGain(pCalculateExposureData->realGain);
        pExposureInfo->analogRealGain             = static_cast<FLOAT>(RegisterToRealGain(pExposureInfo->analogRegisterGain));
        pExposureInfo->digitalRegisterGain        =
            CalculateDigitalGain(pCalculateExposureData->realGain, pExposureInfo->analogRealGain, TRUE);
        pExposureInfo->digitalRealGain            =
            static_cast<FLOAT>(pExposureInfo->digitalRegisterGain / IMX576DigitalGainDecimator);
        pExposureInfo->lineCount                  = pCalculateExposureData->lineCount;
        pExposureInfo->shortRegisterGain          = RealToRegisterGain(pCalculateExposureData->shortRealGain);
        pExposureInfo->shortAnalogRealGain        = static_cast<FLOAT>(RegisterToRealGain(pExposureInfo->shortRegisterGain));
        pExposureInfo->shortDigitalRegisterGain   =
            CalculateDigitalGain(pCalculateExposureData->shortRealGain, pExposureInfo->shortAnalogRealGain, TRUE);
        pExposureInfo->shortDigitalRealGain       =
            static_cast<FLOAT>(pExposureInfo->shortDigitalRegisterGain / IMX576DigitalGainDecimator);
        pExposureInfo->shortLinecount             = pCalculateExposureData->shortLinecount;
        pExposureInfo->middleRegisterGain         = RealToRegisterGain(pCalculateExposureData->middleRealGain);
        pExposureInfo->middleAnalogRealGain       = static_cast<FLOAT>(RegisterToRealGain(pExposureInfo->middleRegisterGain));
        pExposureInfo->middleDigitalRegisterGain  =
            CalculateDigitalGain(pCalculateExposureData->middleRealGain, pExposureInfo->middleAnalogRealGain, TRUE);
        pExposureInfo->middleDigitalRealGain      =
            static_cast<FLOAT>(pExposureInfo->middleDigitalRegisterGain / IMX576DigitalGainDecimator);
        pExposureInfo->middleLinecount            = pCalculateExposureData->middleLinecount;
        pExposureInfo->ISPDigitalGain             = 1.0f;
    }

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
    UINT16  regCount  = 0;

    if ((NULL == pRegSettingsInfo) || (NULL == pExposureData))
    {
        return FALSE;
    }
    if (pExposureData->lineCount < 4)
    {
        pExposureData->lineCount = 4;
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
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->lineCount & 0xFF00) >> 8;
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->coarseIntgTimeAddr + 1;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->lineCount & 0xFF);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->globalAnalogGainAddr;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->analogRegisterGain & 0xFF00) >> 8;
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->globalAnalogGainAddr+ 1;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->analogRegisterGain & 0xFF);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->GlobalDigitalGainAddr;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->digitalRegisterGain & 0xFF00) >> 8;
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->GlobalDigitalGainAddr + 1;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->digitalRegisterGain & 0xFF);
    regCount++;
    if (TRUE == pExposureData->applyShortExposure && TRUE == pExposureData->applyMiddleExposure)
    {
        pRegSettingsInfo->regSetting[regCount].registerAddr  = 0x0202;
        pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->lineCount & 0xFF00) >> 8;
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr  = 0x0202 + 1;
        pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->lineCount & 0xFF);
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr  = 0x0204;
        pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->analogRegisterGain & 0xFF00) >> 8;
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr  = 0x0204 + 1;
        pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->analogRegisterGain & 0xFF);
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr  = 0x020E;
        pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->digitalRegisterGain & 0xFF00) >> 8;
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr  = 0x020E + 1;
        pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->digitalRegisterGain & 0xFF);
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x224;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortLineCount & 0xFF00) >> 8;
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x224 + 1;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortLineCount & 0xFF);
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x216;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortRegisterGain & 0xFF00) >> 8;
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x216 + 1;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortRegisterGain & 0xFF);
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x218;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortDigitalRegisterGain & 0xFF00) >> 8;
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x218 + 1;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortDigitalRegisterGain & 0xFF);
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x3FE0;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->middleLineCount & 0xFF00) >> 8;
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x3FE0 + 1;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->middleLineCount & 0xFF);
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x3FE2;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->middleRegisterGain & 0xFF00) >> 8;
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x3FE2 + 1;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->middleRegisterGain & 0xFF);
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x3FE4;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->middleDigitalRegisterGain & 0xFF00) >> 8;
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x3FE4 + 1;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->middleDigitalRegisterGain & 0xFF);
        regCount++;

    }
    else if (TRUE == pExposureData->applyShortExposure)
    {
        pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->shortCoarseIntgTimeAddr;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortLineCount & 0xFF00) >> 8;
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->shortCoarseIntgTimeAddr + 1;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortLineCount & 0xFF);
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->shortGlobalAnalogGainAddr;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortRegisterGain & 0xFF00) >> 8;
        regCount++;
        pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->shortGlobalAnalogGainAddr + 1;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortRegisterGain & 0xFF);
        regCount++;
    }

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
// FillAutoWhiteBalanceSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL FillAutoWhiteBalanceSettings(
    RegSettingsInfo*    pRegSettingsInfo,
    FLOAT               AWBGainRed,
    FLOAT               AWBGainGreen,
    FLOAT               AWBGainBlue)
{
    UINT32  index      = 0;
    UINT16  regCount   = 0;
    UINT32  rGain = static_cast<UINT32>(AWBGainRed * 256);
    UINT32  gGain = static_cast<UINT32>(AWBGainGreen * 256);
    UINT32  bGain = static_cast<UINT32>(AWBGainBlue * 256);

    if (NULL == pRegSettingsInfo)
    {
        return FALSE;
    }
    pRegSettingsInfo->regSetting[regCount].registerAddr  = 0x0B8E;
    pRegSettingsInfo->regSetting[regCount].registerData  = 0x01;
    pRegSettingsInfo->regSetting[regCount].regAddrType   = I2CRegAddressDataTypeWord;
    pRegSettingsInfo->regSetting[regCount].regDataType   = I2CRegAddressDataTypeByte;
    pRegSettingsInfo->regSetting[regCount].delayUs       = 0;
    regCount++;
    pRegSettingsInfo->regSetting[regCount].registerAddr  = 0x0B8F;
    pRegSettingsInfo->regSetting[regCount].registerData  = 0x00;
    pRegSettingsInfo->regSetting[regCount].regAddrType   = I2CRegAddressDataTypeWord;
    pRegSettingsInfo->regSetting[regCount].regDataType   = I2CRegAddressDataTypeByte;
    pRegSettingsInfo->regSetting[regCount].delayUs       = 0;
    regCount++;
    pRegSettingsInfo->regSetting[regCount].registerAddr  = 0x0B90;
    pRegSettingsInfo->regSetting[regCount].registerData  = (rGain & 0xFF00) >> 8;
    pRegSettingsInfo->regSetting[regCount].regAddrType   = I2CRegAddressDataTypeWord;
    pRegSettingsInfo->regSetting[regCount].regDataType   = I2CRegAddressDataTypeByte;
    pRegSettingsInfo->regSetting[regCount].delayUs       = 0;
    regCount++;
    pRegSettingsInfo->regSetting[regCount].registerAddr  = 0x0B91;
    pRegSettingsInfo->regSetting[regCount].registerData  = rGain & 0x00FF;
    pRegSettingsInfo->regSetting[regCount].regAddrType   = I2CRegAddressDataTypeWord;
    pRegSettingsInfo->regSetting[regCount].regDataType   = I2CRegAddressDataTypeByte;
    pRegSettingsInfo->regSetting[regCount].delayUs       = 0;
    regCount++;
    pRegSettingsInfo->regSetting[regCount].registerAddr  = 0x0B92;
    pRegSettingsInfo->regSetting[regCount].registerData  = (bGain & 0xFF00) >> 8;
    pRegSettingsInfo->regSetting[regCount].regAddrType   = I2CRegAddressDataTypeWord;
    pRegSettingsInfo->regSetting[regCount].regDataType   = I2CRegAddressDataTypeByte;
    pRegSettingsInfo->regSetting[regCount].delayUs       = 0;
    regCount++;
    pRegSettingsInfo->regSetting[regCount].registerAddr  = 0x0B93;
    pRegSettingsInfo->regSetting[regCount].registerData  = bGain & 0xFF;
    pRegSettingsInfo->regSetting[regCount].regAddrType   = I2CRegAddressDataTypeWord;
    pRegSettingsInfo->regSetting[regCount].regDataType   = I2CRegAddressDataTypeByte;
    pRegSettingsInfo->regSetting[regCount].delayUs       = 0;
    regCount++;
    pRegSettingsInfo->regSetting[regCount].registerAddr  = 0x0B94;
    pRegSettingsInfo->regSetting[regCount].registerData  = (gGain & 0xFF00) >> 8;
    pRegSettingsInfo->regSetting[regCount].regAddrType   = I2CRegAddressDataTypeWord;
    pRegSettingsInfo->regSetting[regCount].regDataType   = I2CRegAddressDataTypeByte;
    pRegSettingsInfo->regSetting[regCount].delayUs       = 0;
    regCount++;
    pRegSettingsInfo->regSetting[regCount].registerAddr  = 0x0B95;
    pRegSettingsInfo->regSetting[regCount].registerData  = gGain & 0xFF;
    pRegSettingsInfo->regSetting[regCount].regAddrType   = I2CRegAddressDataTypeWord;
    pRegSettingsInfo->regSetting[regCount].regDataType   = I2CRegAddressDataTypeByte;
    pRegSettingsInfo->regSetting[regCount].delayUs       = 0;
    regCount++;
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
// StatsParse
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID StatsParse(
    UINT8*             pInputData,
    VOID*              pOutputData,
    UINT               reserved1,
    UINT               reserved2)
{
    (void)reserved1;
    (void)reserved2;
    StatsHDR3ExposureDataType* pDataParse = reinterpret_cast<StatsHDR3ExposureDataType *>(pOutputData);
    UINT8* pInput = pInputData;

    //IMX576_DBG("Stats Parsing funciton called");
    pDataParse->splitHistchannelCount = 1;
    for (UINT32 channelCount = 0; channelCount < pDataParse->splitHistchannelCount; channelCount++)
    {
        for (UINT32 index = 0; index < 128; index++)
        {
            pDataParse->splitBayerHist.pHistData[channelCount][index] =
                (pInput[index * 5] << 15) + (pInput[index * 5 + 1] << 7) + (pInput[index * 5 + 2] >> 1);
        }
        for (UINT32 index = 0; index < 8; index++)
        {
            pDataParse->splitBayerHist.pHistData[channelCount][126 - index]
                = pDataParse->splitBayerHist.pHistData[channelCount][126 - index] +
                  pDataParse->splitBayerHist.pHistData[channelCount][127 - index];
            pDataParse->splitBayerHist.pHistData[channelCount][126 - index] = 0;
        }
        pDataParse->splitBayerHist.histDataType[channelCount] = StatsColorChannelY;
        pDataParse->splitBayerHist.binCount                   = 128;
        pDataParse->splitBayerHist.totalHistCount             = 6209280;
        pDataParse->splitBayerHist.histWeightUnit             = 1;
        pDataParse->splitBayerHist.measurementRegion          = MeasurementRegionType::GlobalRegion;
        pDataParse->splitBayerHist.mergedType                 = HDR3ExposureMergedType::HDR3ExpSplitLong;
    }


    pDataParse->mergedHistchannelCount = 1;
    pInput = pInputData + 640;
    for (UINT32 channelCount = 0; channelCount < pDataParse->mergedHistchannelCount; channelCount++)
    {
        for (UINT32 index = 0; index < 16; index++)
        {
            pDataParse->mergedBayerHist.pHistData[channelCount][index] =
                (pInput[index * 5] << 15) + (pInput[index * 5 + 1] << 7) + (pInput[index * 5 + 2] >> 1);
        }
        pDataParse->mergedBayerHist.histDataType[channelCount] = StatsColorChannelY;
        pDataParse->mergedBayerHist.binCount                   = 16;
        pDataParse->mergedBayerHist.totalHistCount             = 6209280;
        pDataParse->mergedBayerHist.histWeightUnit             = 1;
        pDataParse->mergedBayerHist.measurementRegion          = MeasurementRegionType::GlobalRegion;
        pDataParse->mergedBayerHist.mergedType                 = HDR3ExposureMergedType::HDR3ExpMerged;
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
    pSensorLibraryAPI->pFillAutoWhiteBalanceSettings = FillAutoWhiteBalanceSettings;
    pSensorLibraryAPI->pStatsParse           = StatsParse;
}

#ifdef __cplusplus
} // CamX Namespace
#endif // __cplusplus
