////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
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

#undef IMX318_DBG
#ifdef IMX318_DBG_ENABLE
#define IMX318_DBG(fmt, args, ...) __android_log_print(ANDROID_LOG_INFO, "IMX318",fmt, ##args)
#else
#define IMX318_DBG(fmt, args, ...)
#endif


#define MAX_FLOATING_WIN 8
#define MAX_FIXED_WIN 192

#define PD_ENABLE_HDR_ADDR 0x3011
#define PD_ENABLE_ADDR 0x3031
#define AREA_MODE_ADDR 0x315d
#define PD_OUT_EN_ADDR 0x3033
#define PD_AREA_X_OFFSET_ADDR 0x3100
#define PD_AREA_Y_OFFSET_ADDR 0x3102
#define PD_AREA_WIDTH_ADDR 0x3104
#define PD_AREA_HEIGHT_ADDR 0x3106
#define FLEX_AREA_EN_ADDR 0x315E
#define FLEX_AREA_XSTA0_ADDR 0x3108
#define CL_SHIFT_ADDR 0x3167
#define HDR_EN_ADDR 0
#define HDR_BINNING_ADDR 0
#define X_ADD_STA_ADDR 0
#define Y_ADD_STA_ADDR 0
#define X_ADD_END_ADDR 0
#define Y_ADD_END_ADDR 0
#define X_OUT_SIZE_ADDR 0
#define Y_OUT_SIZE_ADDR 0
#define X_EVN_INC_ADDR 0
#define X_ODD_INC_ADDR 0
#define Y_EVN_INC_ADDR 0
#define Y_ODD_INC_ADDR 0
#define SCALE_MODE_ADDR 0
#define SCALE_M_ADDR 0
#define DIG_CROP_X_OFFSET_ADDR 0
#define DIG_CROP_Y_OFFSET_ADDR 0
#define DIG_CROP_IMAGE_WIDTH_ADDR 0
#define DIG_CROP_IMAGE_HEIGHT_ADDR 0
#define BINNING_MODE_ADDR 0
#define BINNING_TYPE_ADDR 0

static const UINT   IMX318MinRegisterGain        = 0;                                     ///< Minimum analog register gain
static const UINT   IMX318MaxRegisterGain        = 480;                                   ///< Maximum analog register gain
static const DOUBLE IMX318MinRealGain            = (512 / (512 - IMX318MinRegisterGain)); ///< Minimum analog real gain (1X)
static const DOUBLE IMX318MaxRealGain            = (512 / (512 - IMX318MaxRegisterGain)); ///< Maximum analog real gain (16X)
static const UINT   IMX318MinDigitalRegisterGain = 256;                                   ///< Minimum digital register gain
static const UINT   IMX318MaxDigitalRegisterGain = 256;                                   ///< Maximum digital register gain
static const UINT   IMX318DigitalGainDecimator   = 256;                                   /// < Digital gain decimator factor
static const DOUBLE IMX318MinDigitalRealGain     = (IMX318MinDigitalRegisterGain / IMX318DigitalGainDecimator);
                                                                                          ///< Minimum digital real gain (1X)
static const DOUBLE IMX318MaxDigitalRealGain     = (IMX318MaxDigitalRegisterGain/ IMX318DigitalGainDecimator);
                                                                                          ///< Maximum digital real gain (1X)
static const SensorPDAFInfo IMX318PDAFRegInfo =
{
    PD_ENABLE_HDR_ADDR,             // pd_enable_hdr_addr;
    PD_ENABLE_ADDR,                 // pd_enable_addr;
    AREA_MODE_ADDR,                 // area_mode_addr;
    PD_OUT_EN_ADDR,                 // pd_out_en_addr;
    PD_AREA_X_OFFSET_ADDR,          // pd_area_x_offset_addr;
    PD_AREA_Y_OFFSET_ADDR,          // pd_area_y_offset_addr;
    PD_AREA_WIDTH_ADDR,             // pd_area_width_addr;
    PD_AREA_HEIGHT_ADDR,            // pd_area_height_addr;
    FLEX_AREA_EN_ADDR,              // flex_area_en_addr;
    FLEX_AREA_XSTA0_ADDR,           // flex_area_xsta0_addr;
    2,                              // flex_mode_data;
    1,                              // fixed_mode_data;
    0,                              // orientation_addr;
    HDR_EN_ADDR,                    // hdr_en_addr;
    HDR_BINNING_ADDR,               // hdr_binning_addr;
    X_ADD_STA_ADDR,                 // x_add_sta_addr;
    Y_ADD_STA_ADDR,                 // y_add_sta_addr;
    X_ADD_END_ADDR,                 // x_add_end_addr;
    Y_ADD_END_ADDR,                 // y_add_end_addr;
    X_OUT_SIZE_ADDR,                // x_out_size_addr;
    Y_OUT_SIZE_ADDR,                // y_out_size_addr;
    X_EVN_INC_ADDR,                 // x_evn_inc_addr;
    X_ODD_INC_ADDR,                 // x_odd_inc_addr;
    Y_EVN_INC_ADDR,                 // y_evn_inc_addr;
    Y_ODD_INC_ADDR,                 // y_odd_inc_addr;
    SCALE_MODE_ADDR,                // scale_mode_addr;
    SCALE_M_ADDR,                   // scale_m_addr;
    DIG_CROP_X_OFFSET_ADDR,         // dig_crop_x_offset_addr;
    DIG_CROP_Y_OFFSET_ADDR,         // dig_crop_y_offset_addr;
    DIG_CROP_IMAGE_WIDTH_ADDR,      // dig_crop_image_width_addr;
    DIG_CROP_IMAGE_HEIGHT_ADDR,     // dig_crop_image_height_addr;
    BINNING_MODE_ADDR,              // binning_mode_addr;
    BINNING_TYPE_ADDR,              // binning_type_addr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RegisterToRealGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static DOUBLE RegisterToRealGain(
    UINT registerGain)
{
    if(IMX318MaxRegisterGain < registerGain)
    {
        registerGain = IMX318MaxRegisterGain;
    }

    return  (512.0 / (512.0 - registerGain));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RealToRegisterGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static UINT RealToRegisterGain(
    DOUBLE realGain)
{
    if (IMX318MinRealGain > realGain)
    {
        realGain = IMX318MinRealGain;
    }
    else if (IMX318MaxRealGain < realGain)
    {
        realGain = IMX318MaxRealGain;
    }

    return static_cast<UINT>(512.0 - (512.0 / realGain));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CalculateDigitalGain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static UINT CalculateDigitalGain(
    FLOAT totalRealGain,
    FLOAT sensorRealGain)
{
    float digitalRealGain = IMX318MinDigitalRealGain;

    if (IMX318MaxRealGain < totalRealGain)
    {
        digitalRealGain = totalRealGain / sensorRealGain;
    }
    else
    {
        digitalRealGain = IMX318MinDigitalRealGain;
    }

    if (IMX318MaxDigitalRealGain < digitalRealGain)
    {
        digitalRealGain = IMX318MaxDigitalRealGain;
    }

    return static_cast<UINT>(digitalRealGain * IMX318DigitalGainDecimator);
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

    pExposureInfo->analogRegisterGain   = RealToRegisterGain(pCalculateExposureData->realGain);
    pExposureInfo->analogRealGain       = static_cast<FLOAT>(RegisterToRealGain(pExposureInfo->analogRegisterGain));
    pExposureInfo->digitalRegisterGain  = CalculateDigitalGain(pCalculateExposureData->realGain, pExposureInfo->analogRealGain);
    pExposureInfo->digitalRealGain      = static_cast<FLOAT>(pExposureInfo->digitalRegisterGain / IMX318DigitalGainDecimator);
    pExposureInfo->ISPDigitalGain       =
        pCalculateExposureData->realGain /(pExposureInfo->analogRealGain * pExposureInfo->digitalRealGain);
    pExposureInfo->lineCount            = pCalculateExposureData->lineCount;

    // Since isp channel gain block is common to both short and long, remove isp digital gain from short sensitivity.
    pCalculateExposureData->shortRealGain = pCalculateExposureData->shortRealGain / pExposureInfo->ISPDigitalGain;

    if (pCalculateExposureData->shortRealGain < 1.0f)
    {
        // Considering isp digital gain in exposuretime, since gain exausted.
        pCalculateExposureData->shortLinecount =
            static_cast<UINT>(pCalculateExposureData->shortRealGain * pCalculateExposureData->shortLinecount);
        // Making linecount 1 if it falls less then 1
        pCalculateExposureData->shortLinecount =
            pCalculateExposureData->shortLinecount < 1 ? 1 : pCalculateExposureData->shortLinecount;
        pCalculateExposureData->shortRealGain = 1.0f;
    }
    pExposureInfo->shortRegisterGain    = RealToRegisterGain(pCalculateExposureData->shortRealGain);
    pExposureInfo->shortLinecount       = pCalculateExposureData->shortLinecount;

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

    if (TRUE == pExposureData->applyShortExposure)
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
// FillPDAFSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL FillPDAFSettings(
        RegSettingsInfo*        pRegSettingsInfo,
        SensorFillPDAFData*     pPDAFData)
{
    UINT16  regCount     = 0;
    UINT16  winNumber    = 0;
    UINT    flexAreaADDR = 0;
    BOOL    result       = FALSE;

    if ((NULL == pRegSettingsInfo) || (NULL == pPDAFData))
    {
        return result;
    }

    IMX318_DBG("PDLibROI type %d, [%d %d %d %d], hnum %d vnum %d",
                                                pPDAFData->PDAFroiType,
                                                pPDAFData->PDAFstartX,
                                                pPDAFData->PDAFstartY,
                                                pPDAFData->PDAFendX,
                                                pPDAFData->PDAFendY,
                                                pPDAFData->horizontalWindowCount,
                                                pPDAFData->verticalWindowCount);

        winNumber    = static_cast<UINT16>(pPDAFData->horizontalWindowCount * pPDAFData->verticalWindowCount);
        flexAreaADDR = FLEX_AREA_XSTA0_ADDR;

        if (winNumber > MAX_FIXED_WIN || winNumber == 0)
        {
          return result;
        }

        if (winNumber == 1)
        {

            pRegSettingsInfo->regSetting[regCount].registerAddr = flexAreaADDR;
            pRegSettingsInfo->regSetting[regCount].registerData = ((pPDAFData->PDAFstartX) & 0xFF00)>> 8;
            pRegSettingsInfo->regSetting[regCount].regAddrType  = I2CRegAddressDataTypeWord;
            pRegSettingsInfo->regSetting[regCount].regDataType  = I2CRegAddressDataTypeByte;
            pRegSettingsInfo->regSetting[regCount].delayUs      = 0;
            regCount++;
            flexAreaADDR++;

            IMX318_DBG("Flex Register Addr  = 0x%x Data = 0x%x",flexAreaADDR-1,
                                    pRegSettingsInfo->regSetting[regCount-1].registerData);

            pRegSettingsInfo->regSetting[regCount].registerAddr = flexAreaADDR;
            pRegSettingsInfo->regSetting[regCount].registerData = (pPDAFData->PDAFstartX) & 0xFF;
            pRegSettingsInfo->regSetting[regCount].regAddrType  = I2CRegAddressDataTypeWord;
            pRegSettingsInfo->regSetting[regCount].regDataType  = I2CRegAddressDataTypeByte;
            pRegSettingsInfo->regSetting[regCount].delayUs      = 0;
            regCount++;
            flexAreaADDR++;

            IMX318_DBG("Flex Register Addr  = 0x%x Data = 0x%x",flexAreaADDR-1,
                                    pRegSettingsInfo->regSetting[regCount-1].registerData);

            pRegSettingsInfo->regSetting[regCount].registerAddr = flexAreaADDR;
            pRegSettingsInfo->regSetting[regCount].registerData = (pPDAFData->PDAFstartY & 0xFF00)>> 8;
            pRegSettingsInfo->regSetting[regCount].regAddrType  = I2CRegAddressDataTypeWord;
            pRegSettingsInfo->regSetting[regCount].regDataType  = I2CRegAddressDataTypeByte;
            pRegSettingsInfo->regSetting[regCount].delayUs      = 0;
            regCount++;
            flexAreaADDR++;

            IMX318_DBG("Flex Register Addr  = 0x%x Data = 0x%x",flexAreaADDR-1,
                                    pRegSettingsInfo->regSetting[regCount-1].registerData);

            pRegSettingsInfo->regSetting[regCount].registerAddr = flexAreaADDR;
            pRegSettingsInfo->regSetting[regCount].registerData = pPDAFData->PDAFstartY & 0xFF;
            pRegSettingsInfo->regSetting[regCount].regAddrType  = I2CRegAddressDataTypeWord;
            pRegSettingsInfo->regSetting[regCount].regDataType  = I2CRegAddressDataTypeByte;
            pRegSettingsInfo->regSetting[regCount].delayUs      = 0;
            regCount++;
            flexAreaADDR++;

            IMX318_DBG("Flex Register Addr  = 0x%x Data = 0x%x",flexAreaADDR-1,
                                    pRegSettingsInfo->regSetting[regCount-1].registerData);

            pRegSettingsInfo->regSetting[regCount].registerAddr = flexAreaADDR;
            pRegSettingsInfo->regSetting[regCount].registerData = (pPDAFData->PDAFendX & 0xFF00)>> 8;
            pRegSettingsInfo->regSetting[regCount].regAddrType  = I2CRegAddressDataTypeWord;
            pRegSettingsInfo->regSetting[regCount].regDataType  = I2CRegAddressDataTypeByte;
            pRegSettingsInfo->regSetting[regCount].delayUs      = 0;
            regCount++;
            flexAreaADDR++;

            IMX318_DBG("Flex Register Addr  = 0x%x Data = 0x%x",flexAreaADDR-1,
                                    pRegSettingsInfo->regSetting[regCount-1].registerData);

            pRegSettingsInfo->regSetting[regCount].registerAddr = flexAreaADDR;
            pRegSettingsInfo->regSetting[regCount].registerData = pPDAFData->PDAFendX & 0xFF;
            pRegSettingsInfo->regSetting[regCount].regAddrType  = I2CRegAddressDataTypeWord;
            pRegSettingsInfo->regSetting[regCount].regDataType  = I2CRegAddressDataTypeByte;
            pRegSettingsInfo->regSetting[regCount].delayUs      = 0;
            regCount++;
            flexAreaADDR++;

            IMX318_DBG("Flex Register Addr  = 0x%x Data = 0x%x",flexAreaADDR-1,
                                    pRegSettingsInfo->regSetting[regCount-1].registerData);

            pRegSettingsInfo->regSetting[regCount].registerAddr = flexAreaADDR;
            pRegSettingsInfo->regSetting[regCount].registerData = (pPDAFData->PDAFendY & 0xFF00)>> 8;
            pRegSettingsInfo->regSetting[regCount].regAddrType  = I2CRegAddressDataTypeWord;
            pRegSettingsInfo->regSetting[regCount].regDataType  = I2CRegAddressDataTypeByte;
            pRegSettingsInfo->regSetting[regCount].delayUs      = 0;
            regCount++;
            flexAreaADDR++;

            IMX318_DBG("Flex Register Addr  = 0x%x Data = 0x%x",flexAreaADDR-1,
                                    pRegSettingsInfo->regSetting[regCount-1].registerData);

            pRegSettingsInfo->regSetting[regCount].registerAddr = flexAreaADDR;
            pRegSettingsInfo->regSetting[regCount].registerData = pPDAFData->PDAFendY & 0xFF;
            pRegSettingsInfo->regSetting[regCount].regAddrType  = I2CRegAddressDataTypeWord;
            pRegSettingsInfo->regSetting[regCount].regDataType  = I2CRegAddressDataTypeByte;
            pRegSettingsInfo->regSetting[regCount].delayUs      = 0;
            regCount++;
            flexAreaADDR++;

            IMX318_DBG("Flex Register Addr  = 0x%x Data = 0x%x",flexAreaADDR-1,
                                    pRegSettingsInfo->regSetting[regCount-1].registerData);
            result = TRUE;
        }

    pRegSettingsInfo->regSettingCount = regCount;

    return result;
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
    pSensorLibraryAPI->pFillPDAFSettings     = FillPDAFSettings;
}

#ifdef __cplusplus
} // CamX Namespace
#endif // __cplusplus
