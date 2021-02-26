////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cutils/properties.h>
#include <stdlib.h>
#include "camxsensordriverapi.h"
// NOWHINE ENTIRE FILE
#include <android/log.h>
#include <math.h>
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define IMX334_DBG(fmt, args...) __android_log_print(ANDROID_LOG_INFO, "IMX334",fmt, ##args)
#define VMAX 5000
#define VMAX_DOL3 (1128*4)
#define RHS1_DOL2HDR     609
#define MIN_SHR0_DOL2HDR 618
#define MIN_SHR1_DOL2HDR 9
#define RHS1_DOL3HDR     151
#define RHS2_DOL3HDR     206
#define MIN_SHR0_DOL3HDR 219
#define MIN_SHR1_DOL3HDR 13
#define MIN_SHR2_DOL3HDR 164

static const INT   IMX334MinRegisterGain        = 0;                                     ///< Minimum analog register gain
static const INT   IMX334MaxRegisterGain        = 240;                                   ///< Maximum analog register gain
/* TODO: Check for DGAIN CALC
static const UINT   IMX318MinDigitalRegisterGain = 1;                                   ///< Minimum digital register gain
static const UINT   IMX318MaxDigitalRegisterGain = 140;                                   ///< Maximum digital register gain
static const DOUBLE IMX334MinDigitalRealGain     = ((3 * IMX318MinDigitalRegisterGain) / 10);
                                                                                 ///< Minimum digital real gain (1X)
static const DOUBLE IMX334MaxDigitalRealGain     = ((3 * IMX318MaxDigitalRegisterGain) / 10);
                                                                                          ///< Maximum digital real gain (1X)
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RegisterToRealGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static DOUBLE RegisterToRealGain(
    UINT registerGain)
{
    if(IMX334MaxRegisterGain < registerGain)
    {
        registerGain = IMX334MaxRegisterGain;
    }

    return powf(10, (((static_cast<FLOAT>(registerGain))*0.3)/20.0));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RealToRegisterGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static INT RealToRegisterGain(
    DOUBLE realGain)
{
    INT regGain = (20.0 * log10(realGain) *10.0)/3.0;

    if (IMX334MinRegisterGain > regGain)
    {
        realGain = IMX334MinRegisterGain;
    }
    else if (IMX334MaxRegisterGain < regGain)
    {
        regGain = IMX334MaxRegisterGain;
    }

    return regGain;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FillExposureSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CalculateExposure(
     SensorExposureInfo* pExposureInfo,
     SensorCalculateExposureData* pCalculateExposureData)
{

    if (NULL == pExposureInfo)
    {
        return FALSE;
    }

    FLOAT devGain_long;
    FLOAT devGain_short;
    FLOAT devGain_middle;
    FLOAT tmpMiddleRealGain = 0.0f;
    uint32_t lineCount       = pCalculateExposureData->lineCount;
    uint32_t shortLinecount  = pCalculateExposureData->shortLinecount;
    uint32_t middleLinecount = pCalculateExposureData->middleLinecount;

    if (pExposureInfo->applyShortExposure == TRUE && pExposureInfo->applyMiddleExposure == TRUE)
    {
        if((lineCount%3 != 0) && (lineCount>1) && (shortLinecount>0)) //long line count can only be even numbers in DOL2 mode
        {
            devGain_long = (FLOAT) lineCount/(lineCount-(lineCount%3));
            pCalculateExposureData->realGain *= devGain_long;
            lineCount -= lineCount%3;
        }

        if ((shortLinecount%3 != 0) && (shortLinecount>1)) //short line count can only be even numbers in DOL2 mode
        {
            devGain_short = (FLOAT) shortLinecount/(shortLinecount-(shortLinecount%3));
            pCalculateExposureData->shortRealGain *=devGain_short;
            shortLinecount -= (shortLinecount%3);
        }

        if ((middleLinecount%3 != 0) && (middleLinecount>1)) //short line count can only be even numbers in DOL2 mode
        {
            devGain_middle = (FLOAT) middleLinecount/(middleLinecount-(middleLinecount%3));
            pCalculateExposureData->middleRealGain *=devGain_middle;
            middleLinecount -= (middleLinecount%3);
        }

        pExposureInfo->analogRegisterGain         = RealToRegisterGain(pCalculateExposureData->realGain);
        pExposureInfo->analogRealGain             = static_cast<FLOAT>(RegisterToRealGain(pExposureInfo->analogRegisterGain));
        pExposureInfo->digitalRegisterGain        = 1;
        pExposureInfo->digitalRealGain            = 1;
        pExposureInfo->lineCount                  = lineCount;
        pExposureInfo->shortRegisterGain          = RealToRegisterGain(pCalculateExposureData->shortRealGain);
        pExposureInfo->shortAnalogRealGain        = static_cast<FLOAT>(RegisterToRealGain(pExposureInfo->shortRegisterGain));
        pExposureInfo->shortDigitalRegisterGain   = 1;
        pExposureInfo->shortDigitalRealGain       = 1;
        pExposureInfo->shortLinecount             = shortLinecount;
        tmpMiddleRealGain                         = static_cast<FLOAT>(pCalculateExposureData->middleRealGain) / 100.0f;
        pExposureInfo->middleRegisterGain         = RealToRegisterGain(tmpMiddleRealGain);
        pExposureInfo->middleAnalogRealGain       = static_cast<FLOAT>(RegisterToRealGain(pExposureInfo->middleRegisterGain));
        pExposureInfo->middleDigitalRegisterGain  = 1;
        pExposureInfo->middleDigitalRealGain      = 1;
        pExposureInfo->middleLinecount            = pCalculateExposureData->middleLinecount;
    }
    else

    {
        if((lineCount%2==1) && (lineCount>1) && (pCalculateExposureData->shortLinecount>0)) //long line count can only be even numbers in DOL2 mode
        {
            devGain_long = (FLOAT) lineCount/(lineCount-1);
            pCalculateExposureData->realGain *= devGain_long;
            lineCount--;
        }

        if ((shortLinecount%2==1) && (shortLinecount>1)) //short line count can only be even numbers in DOL2 mode
        {
            devGain_short = (FLOAT) shortLinecount/(shortLinecount-1);
            pCalculateExposureData->shortRealGain *=devGain_short;
            shortLinecount--;
        }

        pExposureInfo->analogRegisterGain   = RealToRegisterGain(pCalculateExposureData->realGain);
        pExposureInfo->analogRealGain       = static_cast<FLOAT>(RegisterToRealGain(pExposureInfo->analogRegisterGain));
        pExposureInfo->digitalRegisterGain  = 1; // TODO: Check for DGAIN CAL
        pExposureInfo->digitalRealGain      = 1;
        pExposureInfo->ISPDigitalGain       = pCalculateExposureData->realGain /(pExposureInfo->analogRealGain * pExposureInfo->digitalRealGain);
        pExposureInfo->lineCount            = lineCount;
        pExposureInfo->shortRegisterGain    = RealToRegisterGain(pCalculateExposureData->shortRealGain);
        pExposureInfo->shortLinecount       = shortLinecount;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetExposureTime
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT64 GetExposureTime(
    UINT64              algoType,
    INT32*              additionalInfo)
{
    if (2 == algoType)
    {
        additionalInfo[0] = 2;
        additionalInfo[1] = (RHS1_DOL2HDR - MIN_SHR1_DOL2HDR);

        additionalInfo[2] = additionalInfo[0];
        additionalInfo[3] = additionalInfo[1];

        additionalInfo[4] = 2;
        additionalInfo[5] =  VMAX - (RHS1_DOL2HDR + 9);
    }

    if (3 == algoType)
    {
        additionalInfo[0] = 3;
        additionalInfo[1] = (RHS1_DOL3HDR - MIN_SHR1_DOL3HDR);

        additionalInfo[2] = 3;
        additionalInfo[3] = RHS2_DOL3HDR - (RHS1_DOL3HDR + MIN_SHR1_DOL3HDR);

        additionalInfo[4] = 3;
        additionalInfo[5] = VMAX_DOL3 - (RHS2_DOL3HDR + MIN_SHR1_DOL3HDR);

    }

    return algoType;
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
    INT     shr_lef = 0;
    INT     shr_mef = 0;
    INT     shr_sef = 0;
    UINT32 middleLineCount = 0;
    UINT32 middleRegisterGain = 0;
//    UINT32 m_linecount = UINT32((sqrt((DOUBLE)pExposureData->lineCount) * sqrt((DOUBLE)pExposureData->shortLineCount)));
//    UINT32 m_again = UINT32((sqrt((DOUBLE)pExposureData->analogRegisterGain) * sqrt((DOUBLE)pExposureData->shortRegisterGain)));

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
    if (TRUE == pExposureData->applyShortExposure && FALSE == pExposureData->applyMiddleExposure)
    {
        pExposureData->frameLengthLines = (VMAX/2);
    }
    else if (TRUE == pExposureData->applyMiddleExposure)
    {
        pExposureData->frameLengthLines = VMAX_DOL3/4;
    }
    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->frameLengthLinesAddr;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->frameLengthLines & 0xFF);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->frameLengthLinesAddr + 1;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->frameLengthLines & 0xFF00) >> 8;
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->frameLengthLinesAddr + 2;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->frameLengthLines & 0xF0000) >> 16;
    regCount++;
    if (FALSE == pExposureData->applyShortExposure)
    {
        pExposureData->lineCount = pExposureData->frameLengthLines - pExposureData->lineCount;

        pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->coarseIntgTimeAddr;
        pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->lineCount & 0xFF);
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->coarseIntgTimeAddr + 1;
        pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->lineCount & 0xFF00) >> 8;
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->coarseIntgTimeAddr + 2;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->lineCount & 0xF0000) >> 16;
        regCount++;
    }
    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->globalAnalogGainAddr;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->analogRegisterGain & 0xFF);
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->globalAnalogGainAddr+ 1;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->analogRegisterGain & 0xFF00) >> 8;
    regCount++;

    /* TODO: Need to check for DGAIN CALC
    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->GlobalDigitalGainAddr;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->digitalRegisterGain & 0xFF00) >> 8;
    regCount++;

    pRegSettingsInfo->regSetting[regCount].registerAddr  = pExposureData->pRegInfo->GlobalDigitalGainAddr + 1;
    pRegSettingsInfo->regSetting[regCount].registerData  = (pExposureData->digitalRegisterGain & 0xFF);
    regCount++;
    */
    if (TRUE == pExposureData->applyShortExposure && TRUE == pExposureData->applyMiddleExposure)
    {
        middleLineCount = pExposureData->middleLineCount;
        middleRegisterGain = pExposureData->middleRegisterGain;
        shr_lef = (VMAX_DOL3 - pExposureData->lineCount);
        shr_mef = (RHS1_DOL3HDR - middleLineCount);
        shr_sef = (RHS2_DOL3HDR - pExposureData->shortLineCount);

        // SEF: SHORT FRAME EXPOSURE LIMITATION
        if (shr_sef < MIN_SHR2_DOL3HDR) {
            shr_sef = MIN_SHR2_DOL3HDR;
        }
        else if (shr_sef > (RHS2_DOL3HDR - 3) ) {
            shr_sef = (RHS2_DOL3HDR - 3);
        }

        // MEF: MIDDLE FRAME EXPOSURE LIMITATION
        if (shr_mef < MIN_SHR1_DOL3HDR) {
            shr_mef = MIN_SHR1_DOL3HDR;
        }
        else if (shr_mef > (RHS1_DOL3HDR - 3) ) {
            shr_mef = (RHS1_DOL3HDR - 3);
        }

        // LEF: LONG FRAME EXPOSURE LIMITATION
        if (shr_lef < MIN_SHR0_DOL3HDR) {
            shr_lef = MIN_SHR0_DOL3HDR;
        } else if (shr_lef > (INT)(VMAX_DOL3 - 3)) {
            shr_lef = (VMAX_DOL3 - 3);
        }

        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x3060;
        pRegSettingsInfo->regSetting[regCount].registerData = (shr_sef & 0xFF);
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x3061;
        pRegSettingsInfo->regSetting[regCount].registerData = (shr_sef & 0xFF00) >> 8;
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x305C;
        pRegSettingsInfo->regSetting[regCount].registerData = (shr_mef & 0xFF);
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x305D;
        pRegSettingsInfo->regSetting[regCount].registerData = (shr_mef & 0xFF00) >> 8;
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x3058;
        pRegSettingsInfo->regSetting[regCount].registerData = (shr_lef & 0xFF);
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x3059;
        pRegSettingsInfo->regSetting[regCount].registerData = (shr_lef & 0xFF00) >> 8;
        regCount++;

        // SET MIDDLE ANALOG GAIN
        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x30EA;
        pRegSettingsInfo->regSetting[regCount].registerData = (middleRegisterGain & 0xFF);
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x30EB;
        pRegSettingsInfo->regSetting[regCount].registerData = (middleRegisterGain & 0xFF00) >> 8;
        regCount++;

        // SET SHORT ANALOG GAIN
        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x30EC; //pExposureData->pRegInfo->shortGlobalAnalogGainAddr;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortRegisterGain & 0xFF);
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x30ED; //pExposureData->pRegInfo->shortGlobalAnalogGainAddr + 1;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortRegisterGain & 0xFF00) >> 8;
        regCount++;



        IMX334_DBG("frame length lines: %d, exposure first:second:third %d:%d:%d, gain first:second:third %d:%d:%d",
                pExposureData->frameLengthLines,
                shr_lef,
                shr_mef,
                shr_sef,
                pExposureData->analogRegisterGain,
                middleRegisterGain,
                pExposureData->shortRegisterGain);
    }
    else if (TRUE == pExposureData->applyShortExposure)
    {
        // for long exp SHR 3058 and short exp SHR 305C

        shr_lef = (VMAX - pExposureData->lineCount);
        shr_sef = (RHS1_DOL2HDR - pExposureData->shortLineCount);

        if (shr_sef < MIN_SHR1_DOL2HDR) {
            shr_sef = MIN_SHR1_DOL2HDR;
        }
        else if (shr_sef > (RHS1_DOL2HDR -2) ) {
            shr_sef = (RHS1_DOL2HDR -2);
        }

        if (shr_lef < MIN_SHR0_DOL2HDR) {
            shr_lef = MIN_SHR0_DOL2HDR;
        }

        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x305C;
        pRegSettingsInfo->regSetting[regCount].registerData = (shr_sef & 0xFF);
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x305D;
        pRegSettingsInfo->regSetting[regCount].registerData = (shr_sef & 0xFF00) >> 8;
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x3058;
        pRegSettingsInfo->regSetting[regCount].registerData = (shr_lef & 0xFF);
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr = 0x3059;
        pRegSettingsInfo->regSetting[regCount].registerData = (shr_lef & 0xFF00) >> 8;
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->shortGlobalAnalogGainAddr;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortRegisterGain & 0xFF);
        regCount++;

        pRegSettingsInfo->regSetting[regCount].registerAddr = pExposureData->pRegInfo->shortGlobalAnalogGainAddr + 1;
        pRegSettingsInfo->regSetting[regCount].registerData = (pExposureData->shortRegisterGain & 0xFF00) >> 8;
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
// GetSensorLibraryAPIs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID GetSensorLibraryAPIs(
    SensorLibraryAPI* pSensorLibraryAPI)
{
    pSensorLibraryAPI->majorVersion          = 1;
    pSensorLibraryAPI->minorVersion          = 0;
    pSensorLibraryAPI->pCalculateExposure    = CalculateExposure;
    pSensorLibraryAPI->pFillExposureSettings = FillExposureSettings;
    pSensorLibraryAPI->pGetExposureTime      = GetExposureTime;
}

#ifdef __cplusplus
} // CamX Namespace
#endif // __cplusplus
