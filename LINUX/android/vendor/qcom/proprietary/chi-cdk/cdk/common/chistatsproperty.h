////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file chistatsproperty.h
/// @brief Define Qualcomm Technologies, Inc. stats proprietary data for holding external/internal properties/events
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHISTATSPROPERTY_H
#define CHISTATSPROPERTY_H

#include "chistatspropertydefines.h"

#include "camxcdktypes.h"
#include "chipdlibcommon.h"

#ifdef __cplusplus

extern "C"
{
#endif // __cplusplus

/// @brief Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

/// @brief Defines the AEC flash type
typedef enum
{
    FlashInfoTypeOff,                   ///< Flash off case
    FlashInfoTypePre,                   ///< Flash is in preflash  case
    FlashInfoTypeMain,                  ///< Flash in in mainflash case
    FlashInfoTypeCalibration,           ///< Flash in dual led calibaration case
    FlashInfoTypeMax = 0x7FFFFFFF       ///< Anchor to indicate the last item in the defines
} AECFlashInfoType;

/// @brief Stats operation mode
typedef enum
{
    StatsOperationModeInvalid           = -1, ///< Stats operation mode: Invalid
    StatsOperationModeNormal            = 0,  ///< Stats operation mode: Normal
    StatsOperationModeFastConvergence   = 1   ///< Stats operation mode: Fast Convergence
} StatsOperationMode;

/// @brief AEC Startup mode
typedef enum
{
    AECStartupModeInvalid         = -1, ///< AEC Startup mode: Invalid
    AECStartupModeNormal          =  0, ///< AEC Startup mode: Normal
    AECStartupModeFastConvergence =  1, ///< AEC Startup mode: Fast Convergence
    AECStartupModeWarmStart       =  2  ///< AEC Startup mode: Warm Start
} AECStartupMode;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes Auto Exposure Control (AEC) Frame Control information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    // Internal Member Variables
    AECExposureData        exposureInfo[ExposureIndexCount]; ///< Exposure info including gain and exposure time;
                                                             ///< consumed by ISP & sensor
    AECExposureData        realExposureInfo[ExposureIndexCount]; ///< Exposure info including gain and exposure time;
                                                                 ///< consumed by CHI to pass to SHDR lib
    FLOAT                  luxIndex;                         ///< Future frame lux index,  consumed by ISP
    AECFlashInfoType       flashInfo;                        ///< Flash information if it is main or preflash
    PreFlashState          preFlashState;                    ///< Preflash state from AEC, consumed by Sensor
    CalibrationFlashState  calibFlashState;                  ///< Calibration state from AEC, consumed by Sensor
    FLOAT                  LEDFirstEntryRatio;               ///< ratio of LED entry w.r.t first entry
    FLOAT                  LEDLastEntryRatio;                ///< ratio of LED entry w.r.t last entry
    UINT32                 LEDCurrents[LEDSettingCount];     ///< The LED currents value for the use case of LED snapshot
    FLOAT                  LEDInfluenceRatio;                ///< The sensitivity ratio which is calculated from
                                                             ///  sensitivity with no flash / sensitivity with
                                                             ///  preflash
    INT32                 IRLedActivated;                   ///< IR LED Activated
    FLOAT                 IRLedIntensity;                   ///< IR LED intensity
} AECFrameControl;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes Auto Exposure Control (AEC) Frame Control information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    // External Member Variables
    AECConfig   statsConfig;    ///< Future frame AEC configuration; consumed by ISP
} AECStatsControl;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes Auto White Balance (AWB) Frame Control information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    // Internal Member Variables
    AWBGainParams   AWBGains;            ///< R/G/B gains
    UINT32          colorTemperature;    ///< Color temperature
    AWBCCMParams    AWBCCM[MaxCCMs];     ///< Color Correction Matrix Value
    UINT32          numValidCCMs;        ///< Number of Valid CCMs
} AWBFrameControl;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes Auto White Balance (AWB) Stats Control information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    // External Member Variables
    AWBConfig        statsConfig;         ///< BG stats configuration

} AWBStatsControl;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes Auto Focus (AF) Frame Control information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    RectangleCoordinate    ROICoordinate;                 ///< Default ROI rectangle coordinates
    MoveLensOutput         moveLensOutput;                ///< Default lens move information
    BOOL                   exposureCompensationEnable;    ///< Exposure compenstation enable
    FOVCOutput             fovcOutput;                    ///< FOVC factor
    PDLibWindowConfig      PDLibROI;                      ///< ROI info for Type1 sensor
} AFFrameControl;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes Auto Focus (AF) Stats Control information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    // External Member Variables
    AFConfigParams  statsConfig;                    ///< BAF stat configuration
} AFStatsControl;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes Fast Convergence Stats Control information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    FLOAT                   sensitivity[ExposureIndexCount];            ///< AEC: Initial sensitivity information
    AECStartupMode          startMode;                                  ///< AEC startup mode
} AECStartupSensitivity;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes Fast Convergence Stats Control information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    StatsOperationMode      operationMode;                              ///< Stats operation mode
    AECStartupSensitivity   aecStartSensitivity;                        ///< AEC: Initial sensitivity information
    AWBFrameControl         awbFrameControl;                            ///< AWB: Initial startup gains
} StatsStreamInitConfig;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes Auto Flicker Detection (AFD) Frame Control information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    // External Member Variables
    AFDStatsRSConfig    statsConfig;    ///< Row Sum stat configuration
} AFDStatsControl;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes Image Histogram (IHist) Frame Control information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    // External Member Variables
    IHistStatsConfig    statsConfig;    ///< IHist stats configuration. if Zero ROI, IHist will use default ROI (full)
} IHistStatsControl;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes Column Sum (CS) Frame Control information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    // External Member Variables
    CSStatsConfig    statsConfig;    ///< CS stats configuration
} CSStatsControl;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHISTATSPROPERTY_H
