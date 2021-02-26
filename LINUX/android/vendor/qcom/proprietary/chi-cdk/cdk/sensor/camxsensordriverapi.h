////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxsensordriverapi.h
/// @brief Sensor Driver function declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXSENSORDRIVERAPI_H
#define CAMXSENSORDRIVERAPI_H

#include "camxcdktypes.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// @ maximum size of the settings to be programmed
#define MAX_REG_SETTINGS                50  ///< Maximum number of register settings allowed to write
                                            /// exposure info including group on and group off
#define MAX_GROUP_HOLD_REG_SETTINGS     10  ///< Maximum number of group hold settings allowed
#define MAX_REG_CONTROL_EXT_DATA_SIZE   16  ///< Max data size of the additional data from external module

/// @brief Enumeration of IO Operations types
typedef enum IOOperationType
{
    IOOperationTypeWrite = 0,        ///< Write single data item to specified address
    IOOperationTypeWriteBurst,       ///< Write a burst of data
    IOOperationTypeWriteSequential,  ///< Write a series of consecutive (addr, val1), (addr+1, val2), (addr+2, val3) pairs
    IOOperationTypeRead,             ///< Read
    IOOperationTypePoll,             ///< Poll
    IOOperationTypeMax               ///< Max
} IOOperationType;

/// @brief Enumeration of supported I2C register address and data types
typedef enum I2CRegAddressDataType
{
    I2CRegAddressDataTypeByte       = 1, ///< Byte: 1 byte
    I2CRegAddressDataTypeWord       = 2, ///< Word: 2 bytes
    I2CRegAddressDataType3Byte      = 3, ///< 3Byte: 3 bytes
    I2CRegAddressDataTypeDoubleWord = 4, ///< DoubleWord: 4 bytes
    I2CRegAddressDataTypeMax
} I2CRegAddressDataType;

struct RegDataSequence
{
    UINT  count;  ///< Number of data items
    UINT* data;   ///< Array of data
};

typedef struct RegSetting
{
    UINT                  registerAddr; ///< register Address
    I2CRegAddressDataType regAddrType;  ///< register address width in bytes
    I2CRegAddressDataType regDataType;  ///< register data width in bytes
    IOOperationType       operation;    ///< IO operation type
    UINT                  delayUs;      ///< delay in micro seconds after IO operation
    union
    {
        UINT                    registerData;    ///< register Data for single random write
        struct RegDataSequence  registerDataSeq; ///< data sequence for burst/sequential command
    };
} RegSetting;

/// @brief Register setting information
typedef struct RegSettingsInfo
{
    UINT        regSettingCount;                ///< Number of valid settings
    RegSetting  regSetting[MAX_REG_SETTINGS];   ///< Array of register settings to be programmed
} RegSettingsInfo;

/// @brief Group hold on Register setting information
typedef struct GroupHoldOnRegSettingsInfo
{
    UINT        regSettingCount;                        ///< Number of valid settings
    RegSetting  regSetting[MAX_GROUP_HOLD_REG_SETTINGS];  ///< Array of register settings to be programmed
} GroupHoldOnRegSettingsInfo;

/// @brief Group hold off Register setting information
typedef struct GroupHoldOffRegSettingsInfo
{
    UINT        regSettingCount;                        ///< Number of valid settings
    RegSetting  regSetting[MAX_GROUP_HOLD_REG_SETTINGS]; ///< Array of register settings to be programmed
} GroupHoldOffRegSettingsInfo;

/// @brief Sensor exposure settings information
typedef struct SensorExposureInfo
{
    FLOAT   analogRealGain;            ///< Real value of the sensor analog gain
    UINT    analogRegisterGain;        ///< Register value of the sensor Analog gain
    FLOAT   digitalRealGain;           ///< Real value of the sensor digital gain
    UINT    digitalRegisterGain;       ///< Register value of the sensor digital gain
    FLOAT   shortAnalogRealGain;       ///< Real value of the sensor short analog gain
    UINT    shortRegisterGain;         ///< Register value for the short Analog gain
    FLOAT   shortDigitalRealGain;      ///< Real value of the sensor short digital gain
    UINT    shortDigitalRegisterGain;  ///< Register value of the sensor short digital gain
    FLOAT   middleAnalogRealGain;      ///< Real value of the sensor middle analog gain
    UINT    middleRegisterGain;        ///< Register value for the middle analog gain
    FLOAT   middleDigitalRealGain;     ///< Real value of the sensor middle digital gain
    UINT    middleDigitalRegisterGain; ///< Register value of the sensor middle digital gain
    FLOAT   ISPDigitalGain;            ///< Digital gain to be consumed by ISP
    UINT    lineCount;                 ///< Line count / coarse integration time
    UINT    shortLinecount;            ///< Short Coarse integration time
    UINT    middleLinecount;           ///< Middle Coarse integration time
    BOOL    applyShortExposure;        ///< if true apply short exposure
    BOOL    applyMiddleExposure;       ///< if true apply middle exposure
} SensorExposureInfo;

/// @brief Sensor exposure register settings information
typedef struct SensorExposureRegInfo
{
    GroupHoldOnRegSettingsInfo  groupHoldOnSettings;        ///< Group hold on Register address settings information
    GroupHoldOffRegSettingsInfo groupHoldOffSettings;       ///< Group hold off Register address settings information
    UINT32                      frameLengthLinesAddr;       ///< Register address to program Frame length lines
    UINT32                      coarseIntgTimeAddr;         ///< Register address to program coarse integration time
    UINT32                      shortCoarseIntgTimeAddr;    ///< Register address to program short coarse integration time
    UINT32                      globalAnalogGainAddr;       ///< Register address to program analog gain channel
    UINT32                      shortGlobalAnalogGainAddr;  ///< Register address to program short analog gain channel
    UINT32                      GlobalDigitalGainAddr;      ///< Register address to program digital gain channel
    UINT32                      digitalGainRedAddr;         ///< Register address to program digital gain for Red channel
    UINT32                      digitalGainGreenRedAddr;    ///< Register address to program digital gain for green red channel
    UINT32                      digitalGainBlueAddr;        ///< Register address to program digital gain for blue channel
    UINT32                      digitalGainGreenBlueAddr;   ///< Register address to program digitalgain for green blue channel
} SensorExposureRegInfo;

/// @brief Sensor exposure data information to calculate expsoure reg settings
typedef struct SensorCalculateExposureData
{
    FLOAT   realGain;
    UINT    lineCount;
    FLOAT   middleRealGain;
    UINT    middleLinecount;
    FLOAT   shortRealGain;
    UINT    shortLinecount;
} SensorCalculateExposureData;

/// @brief Sensor exposure data information to fill exposure reg settings array
typedef struct SensorFillExposureData
{
    SensorExposureRegInfo* pRegInfo;
    UINT                   analogRegisterGain;
    UINT                   digitalRegisterGain;
    UINT                   lineCount;
    UINT                   frameLengthLines;
    UINT                   shortRegisterGain;
    UINT                   shortDigitalRegisterGain;
    UINT                   middleRegisterGain;
    UINT                   middleDigitalRegisterGain;
    UINT                   shortLineCount;
    UINT                   middleLineCount;
    INT32                  additionalInfo[MAX_REG_CONTROL_EXT_DATA_SIZE];
    BOOL                   applyShortExposure;
    BOOL                   applyMiddleExposure;
} SensorFillExposureData;

/// @brief Sensor PDAF data information to fill PDAF reg settings array for FIXED ROI
typedef struct SensorFillPDAFData
{
    UINT                   PDAFWindowMode;
    UINT                   PDAFroiType;
    UINT                   PDAFstartX;
    UINT                   PDAFstartY;
    UINT                   PDAFendX;
    UINT                   PDAFendY;
    UINT                   horizontalWindowCount;
    UINT                   verticalWindowCount;
} SensorFillPDAFData;

/// @brief Sensor WB gain data information to fill WB gain reg settings array
typedef struct SensorFillWBGainData
{
    FLOAT                  RGain;
    FLOAT                  GGain;
    FLOAT                  BGain;
}SensorFillWBGainData;

/// @breif Sensor PDAF sensor addr for fixed and flexible window
typedef struct {
    unsigned short pd_enable_hdr_addr;
    unsigned short pd_enable_addr;
    unsigned short area_mode_addr;
    unsigned short pd_out_en_addr;
    unsigned short pd_area_x_offset_addr;
    unsigned short pd_area_y_offset_addr;
    unsigned short pd_area_width_addr;
    unsigned short pd_area_height_addr;
    unsigned short flex_area_en_addr;
    unsigned short flex_area_xsta0_addr;
    unsigned short flex_mode_data;
    unsigned short fixed_mode_data;
    unsigned short orientation_addr;
    unsigned short hdr_en_addr;
    unsigned short hdr_binning_addr;
    unsigned short x_add_sta_addr;
    unsigned short y_add_sta_addr;
    unsigned short x_add_end_addr;
    unsigned short y_add_end_addr;
    unsigned short x_out_size_addr;
    unsigned short y_out_size_addr;
    unsigned short x_evn_inc_addr;
    unsigned short x_odd_inc_addr;
    unsigned short y_evn_inc_addr;
    unsigned short y_odd_inc_addr;
    unsigned short scale_mode_addr;
    unsigned short scale_m_addr;
    unsigned short dig_crop_x_offset_addr;
    unsigned short dig_crop_y_offset_addr;
    unsigned short dig_crop_image_width_addr;
    unsigned short dig_crop_image_height_addr;
    unsigned short binning_mode_addr;
    unsigned short binning_type_addr;
} SensorPDAFInfo;

/// @brief APIs in sensor library
typedef struct SensorLibraryAPI
{
    UINT  size;         ///< Size of sensor library API structure
    UINT  majorVersion; ///< Major version of the Library API
    UINT  minorVersion; ///< Minor version of the Library API

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CalculateExposure
    ///
    /// @brief  Calculates and outputs the sensor exposure information based on the inputs provided.
    ///
    /// @param  pExposureInfo             pointer to calculated exposure info to be outputted
    /// @param  pCalculateExposureData    pointer to sensor exposure information to calcualate expsoure reg settings
    ///
    /// @return 1 if successful else 0
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL (*pCalculateExposure)(
        SensorExposureInfo*          pExposureInfo,
        SensorCalculateExposureData* pCalculateExposureData);                 ///< pointer to CalculateExposure function

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FillExposureSettings
    ///
    /// @brief  Fill the exposure settings
    ///
    /// @param  pRegSettingsInfo    pointer to sensor register settings to be filled
    /// @param  pExposureData       pointer to exposure data to fill exposure reg settings
    ///
    /// @return 1 if successful else 0
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL (*pFillExposureSettings)(
        RegSettingsInfo*        pRegSettingsInfo,
        SensorFillExposureData* pExposureData);         ///< pointer to FillExposureSettings function

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FillPDAFSettings
    ///
    /// @brief  Fill the PDAF settings for fixed and flexbile window
    ///
    /// @param  pRegSettingsInfo    pointer to sensor register settings to be filled
    /// @param  pPDAFData           pointer to PDAF data to fill PDAF reg settings
    ///
    /// @return 1 if successful else 0
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL (*pFillPDAFSettings)(
        RegSettingsInfo*        pRegSettingsInfo,
        SensorFillPDAFData*     pPDAFData);         ///< pointer to FillPDAFSettings function

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FillAutoWhiteBalanceSettings
    ///
    /// @brief  Fill the awb settings
    ///
    /// @param  pRegSettingsInfo pointer to sensor settings
    /// @param  AWBGainRed       current AWB Red gain
    /// @param  AWBGainGreen     current AWB Green gain
    /// @param  AWBGainBlue      current AWB Blue gain
    ///
    /// @return 1 if successful else 0
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL (*pFillAutoWhiteBalanceSettings)(
        RegSettingsInfo*    pRegSettingsInfo,
        FLOAT               AWBGainRed,
        FLOAT               AWBGainGreen,
        FLOAT               AWBGainBlue);               ///< pointer to FillAutoWhiteBalanceSettings function

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetExposureTime
    ///
    /// @brief  Get exposure time for report
    ///
    /// @param  currentExposureTime current exposure time
    ///         additionalInfo      pointer of additionalInfo INT32[MAX_REG_CONTROL_EXT_DATA_SIZE]
    ///
    /// @return report exposure time
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT64 (*pGetExposureTime)(
        UINT64              currentExposureTime,
        INT32*              additionalInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetSensitivity
    ///
    /// @brief  Get sensitivity for report
    ///
    /// @param  normalizedSensitivity current exposure time
    ///         additionalInfo        pointer of additionalInfo INT32[MAX_REG_CONTROL_EXT_DATA_SIZE]
    ///
    /// @return report sensitivity
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 (*pGetSensitivity)(
        FLOAT               normalizedSensitivity,
        INT32*              additionalInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// pStatsParse
    ///
    /// @brief  HDR stats parse
    ///
    /// @param  pInputData   original data
    ///         pOutputData  parsed data
    ///         reserved1    reserved for future input/outputs
    ///         reserved2    reserved for future input/outputs
    ///
    /// @return NULL
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID (*pStatsParse)(
        UINT8*             pInputData,
        VOID*              pOutputData,
        UINT               reserved1,
        UINT               reserved2);

} SensorLibraryAPI;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetSensorLibraryAPIs
///
/// @brief Returns the Sensor library API structure
///
/// @param pSensorLibraryAPI pointer to the Sensor library API structure.
///
/// This function is the entry point for the library. CamX will query the shared library for this method by name.
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDK_VISIBILITY_PUBLIC void GetSensorLibraryAPIs(
    SensorLibraryAPI* pSensorLibraryAPI);

#ifdef __cplusplus
} // CamX Namespace
#endif // __cplusplus

#endif // CAMXSENSORDRIVERAPI_H
