////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chiawbinterface.h
/// @brief Auto White Balance algorithm interfaces
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NOWHINE FILE GR016: Typedefs must not be used for structs, classes, and enumerations

#ifndef CHIAWBINTERFACE_H
#define CHIAWBINTERFACE_H

#include "chistatsinterfacedefs.h"
#include "chistatsdebug.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

/// Defines geometrical disparity calibration matrix size for dual camera
static const UINT32 AWBAlgoRotationMatrixSize       = 9;
/// Number of AWB decision maps/areas
static const UINT32 AWBAlgoDecisionMapSize          = 64;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describe AWB Algorithm Input Data definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Defines the Flash states
typedef enum
{
    AWBAlgoFlashInvalid             = -1,           ///< Flash state Invalid
    AWBAlgoFlashInactive,                           ///< Flash off/inactive
    AWBAlgoPreFlashStart,                           ///< Estimation Starts; Preflash turns on
    AWBAlgoPreFlashTriggerFD,                       ///< Start FD under preflash.
    AWBAlgoPreFlashTriggerAF,                       ///< Start AF under preflash.
    AWBAlgoPreFlashTriggerAWB,                      ///< Start AWB under preflash.
    AWBAlgoPreFlashCompleteLED,                     ///< Estimation Completes with LED; Preflash turns Off - Capture with flash
    AWBAlgoPreFlashCompleteNoLED,                   ///< Estimation Completes but no LED required; - No flash required.
    AWBAlgoMainFlash,                               ///< Main flash capture in progress
    AWBAlgoTorchFlash,                              ///< Flash light is always on. No preflash sequence
    AWBAlgoLEDMax                   = 0x7FFFFFFF    ///< Anchor to indicate the last item in the defines
} AWBAlgoFlashState;

typedef enum
{
    AWBAlgoExposureInvalid = -1,                  ///< Indicates invalid or there is no multiple sets of expo stats
    AWBAlgoExposureDefault =  0,                  ///< Default exposure when we don't have multi-set, same as short for now
    AWBAlgoExposureLEF,                           ///< Represents the Long exposure information
    AWBAlgoExposureNum,                           ///< Represents the count of different exposure set
} AWBAlgoMultiExposureType;


/// @brief This structure defines the format of the Exposure information input into the AWB algorithm.
typedef struct
{
    FLOAT   luxIndex;      ///< Lux index.
    BOOL    AECSettled;    ///< If true, AEC is currently settled.
    UINT64  frameDuration; ///< frame Duration
} AWBAlgoExposureInformation;

/// @brief Identifies the type of input to AWB algorithm
typedef enum
{
    AWBInputTypeInvalid         = -1,           ///< Invalid type
                                                ///  Payload: NULL & set size to zero.
    AWBInputTypeRequestNumber,                  ///< Request number number type.
                                                ///  Payload type: UINT64
    AWBInputTypeBGStats,                        ///< BG stats input type
                                                ///  Payload type: StatsBayerGrid
    AWBInputTypeAECData,                        ///< AEC data input type
                                                ///  Payload type: AWBAlgoExposureInformation
    AWBInputTypeDebugData,                      ///< Debug data information. This is optional and can be NULL
                                                ///  Payload type: StatsDataPointer
    AWBInputTypeVendorTag,                      ///< Vendor tag data. List of vendor tags AWB algorithm dependent on.
                                                ///  Payload type: StatsVendorTagList
    AWBInputTypeStatsChiHandle,                 ///< Algo needs to use this handle when querying vendor tags
                                                ///  Payload type: CHISTATSHANDLE
    AWBInputTypeCameraInfo,                     ///< Set camera information regarding algo role(master/slave) and camera ID.
                                                ///  Payload type: StatsCameraInfo
    AWBInputTypeBGStatsROI,                     ///< BG stats ROI
                                                ///  Payload type: StatsRectangle
    AWBInputTypeIsOffline,                      ///< Offline BG stats input type
                                                ///  Payload type: BOOL
    AWBInputTypeBGStatsLEF,                     ///< BG stats input type LEF
                                                ///  Payload type: StatsBayerGrid
    AWBInputTypeLastIndex,                      ///< Last valid index of the enum
                                                ///  Payload type: None
    AWBInputTypeMax             = 0x7FFFFFFF,   ///< Max type
} AWBAlgoInputType;

/// @brief Represents AWB inputs base structure to the AWB algorithm, used as part of AWBAlgoInputList
typedef struct
{
    VOID*               pAWBInput;      ///< Pointer to the payload. See AWBAlgoInputType for details.
    UINT32              sizeOfAWBInput; ///< Size of input payload
    AWBAlgoInputType    inputType;      ///< Type of the payload
} AWBAlgoInput;

/// @brief Represents AWB module inputs to the AWB algorithm
typedef struct
{
    AWBAlgoInput*   pAWBInputs; ///< Pointer to AWBAlgoInput array
    UINT32          inputCount; ///< Number of inputs
} AWBAlgoInputList;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describe AWB Algorithm Output Data definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure describing AWB gains for red, green and blue channels
typedef struct
{
    FLOAT   red;    ///< Red gains
    FLOAT   green;  ///< Green gains
    FLOAT   blue;   ///< Blue gains
} AWBAlgoGains;

typedef struct
{
    FLOAT   rg;
    FLOAT   bg;
} AWBAlgoPoint;

/// @brief Represents AWB states type
typedef enum
{
    AWBAlgoStateInvalid     = -1,           ///< Invalid AWB state.
    AWBAlgoStateInactive,                   ///< AWB is not in auto mode, or has not yet started metering.
    AWBAlgoStateSearching,                  ///< AWB doesn't yet have a good set of control values for the current scene.
    AWBAlgoStateConverged,                  ///< AWB has a good set of control values for the current scene.
    AWBAlgoStateLocked,                     ///< AWB has been locked.
    AWBAlgoStateMax         = 0x7FFFFFFF,   ///< Max type
} AWBAlgoState;

/// @brief Represents AWB output type
typedef enum
{
    AWBOutputTypeInvalid            = -1,           ///< Invalid type
                                                    ///  Payload: NULL & set size to zero
    AWBOutputTypeDebugData,                         ///< Debug data used
                                                    ///  Payload: StatsDataPointer
    AWBOutputTypeGains,                             ///< AWB RGB gains
                                                    ///  Payload: AWBAlgoGains
    AWBOutputTypeColorTemperature,                  ///< AWB color temperature in Kelvin (K)
                                                    ///  Payload: UINT32
    AWBOutputTypeIlluminantType,                    ///< AWB illuminant source decision
                                                    ///  Payload: StatsIlluminantType
    AWBOutputTypeSampleDecision,                    ///< AWB array of sample decision
                                                    ///  Payload: StatsIlluminantType array[AWBAlgoDecisionMapSize]
    AWBOutputTypeBGConfig,                          ///< Optional output: If required by AWB set bayer grid (BG) Stats
                                                    ///  configuration.
                                                    ///  Payload: StatsBayerGridBayerExposureConfig
    AWBOutputTypeState,                             ///< AWB algorithm state
                                                    ///  Payload: AWBAlgoState
    AWBOutputTypeMode,                              ///< AWB algorithm mode used for processing
                                                    ///  Payload: AWBAlgoMode
    AWBOutputTypeLock,                              ///< AWB algorithm lock value, whether the algo is lock.
                                                    ///  Payload: BOOL
    AWBOutputTypeVendorTag,                         ///< Vendor tag data. This is the list of vendor tags AWB algorithm wants to
                                                    ///  publish.
                                                    ///  Payload: StatsVendorTagList
    AWBOutputTypeCCM,                               ///< AWB algorithm color correction matrix values.
                                                    ///  Payload: StatsAWBCCMList
    AWBOutputTypeLastIndex,                         ///< Last valid index of the enum
                                                    ///  Payload type: None
    AWBOutputTypeMax                = 0x7FFFFFFF,   ///< Max type
} AWBAlgoOutputType;

/// @brief Represents AWB output from the algorithm
typedef struct
{
    VOID*               pAWBOutput;             ///< Pointer to the payload. See AWBAlgoOutputType for details
                                                ///  Algorithm implementer needs to do a deep copy to the given memory
    UINT32              sizeOfAWBOutput;        ///< Size of input payload pointed to by pAWBOutput
    UINT32              sizeOfWrittenAWBOutput; ///< Size of payload written back. If no output set to zero.
    AWBAlgoOutputType   outputType;             ///< Type of the payload
} AWBAlgoOutput;

/// @brief Represents AWB output list from the algorithm
typedef struct
{
    AWBAlgoOutput*  pAWBOutputs;    ///< Pointer to AWBAlgoOutput elements array.
    UINT32          outputCount;    ///< Number of elements in pAWBOutputs
} AWBAlgoOutputList;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describe AWB Algorithm Get Parameter definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Represents AWB flash estimation state
typedef enum
{
    AWBAlgoFlashEstimationInvalid   = -1,            ///< AWB flash estimation state is invalid.
    AWBAlgoFlashEstimationInactive,                  ///< AWB is not performing any flash estimation.
    AWBAlgoFlashEstimationRunning,                   ///< AWB flash estimation in progress.
    AWBAlgoFlashEstimationDone,                      ///< AWB flash estimation done.
    AWBAlgoFlashEstimationMax       = 0x7FFFFFFF,   ///< Max type
} AWBAlgoFlashEstimationProgress;

/// @brief Defines the AEC flash state
typedef enum
{
    AWBAlgoFlashOff,                ///< Flash is in off state
    AWBAlgoFlashPre,                ///< Flash is in preflash state
    AWBAlgoFlashMain,               ///< Flash in in mainflash state
    AWBAlgoFlashMax = 0x7FFFFFFF    ///< Anchor to indicate the last item in the defines
} AWBAlgoFlashInfoType;

/// @brief Defines flash related information
typedef struct
{
    AWBAlgoFlashState       flashState;         ///< Flash sequence state.
    FLOAT                   LEDInfluenceRatio;  ///< The sensitivity ratio which is calculated from sensitivity with no flash /
                                                ///  sensitivity with pre-flash, main-flash.
    FLOAT                   LEDRGRatio;         ///< The RG ratio when flash is on.
    FLOAT                   LEDBGRatio;         ///< The BG ratio when flash is on.
    AWBAlgoFlashInfoType    flashType;          ///< Indicates if it is pre flash or main flash
} AWBAlgoFlashInformation;

/// @brief Defines AWB warm-start gains and CCT
typedef struct
{
    AWBAlgoGains    gains;                        ///< AWB gains
    FLOAT           correlatedColorTemperature;   ///< CCT: Correlated Color Temperature
} AWBAlgoWarmstartInformation;

/// @brief Indicates the type of AWB Get/Query of parameters
typedef enum
{
    AWBGetParamTypeInvalid                      = -1,           ///< Invalid type
                                                                ///  Payload: NULL & set size to zero.
    AWBGetParamTypeDebugData,                                   ///< Queries the AWB debug data.
                                                                ///  Payload in: NULL.
                                                                ///  Payload out: AWBGetParamOutputTypeDebugData
    AWBGetParamTypeLastOutput,                                  ///< Queries the last valid output.
                                                                ///  Payload in: NULL.
                                                                ///  Payload out: AWBGetParamOutputTypeOutputList
    AWBGetParamTypeFlashOutput,                                 ///< Request AWB flash output data base on last stats and/or
                                                                ///  pre-flash.
                                                                ///  Payload in: NULL
                                                                ///  Payload out: AWBGetParamOutputTypeOutputList
    AWBGetParamTypeSWVersion,                                   ///< Queries the SW version
                                                                ///  Paylaod in: NULL.
                                                                ///  Payload out: AWBGetParamOutputTypeAlgoVersion.
    AWBGetParamTypeIsStatsDataRequired,                         ///< Queries to know if AWB algorithm requires or not to process
                                                                ///  data at this time.
                                                                ///  This will be call before parsing incoming stats,
                                                                ///  i.e. before process is called into the algorithm
                                                                ///  Payload in: NULL.
                                                                ///  Payload out: AWBGetParamOutputTypeStatsRequired
    AWBGetParamTypeFlashEstimationState,                        ///< Queries the current flash estimation state
                                                                ///  Paylaod in: NULL.
                                                                ///  Payload out: AWBAlgoFlashEstimationProgress.
    AWBGetParamTypePublishingVendorTagsInfo,                    ///< The vendor tags published by the algorithm
                                                                ///  Payload in: NULL. Payload out: StatsVendorTagInfoList
    AWBGetParamTypeDependentVendorTags,                         ///< The vendor tags dependent by the algorithm
                                                                ///  Payload in: NULL. Payload out: StatsVendorTagInfoList
    AWBGetParamTypePeerInfo,                                    ///< Multi camera peer information
                                                                ///  Payload in: NULL. Payload out: VOID*
    AWBGetParamTypeCctToGain,                                   ///< Get the AWB gains based on CCT value
    AWBGetParamTypeGainToCCT,                                   ///< Get the CCT value base AWB gains
    AWBGetParamTypeBGConfig,                                    ///< Get AWB stats config
                                                                ///  Payload: StatsBayerGridBayerExposureConfig
    AWBGetParamTypeLastIndex,                                   ///< Last valid index of the enum
                                                                ///  Payload type: None
    AWBGetParamTypeMax                          = 0x7FFFFFFF,   ///< Max type
} AWBAlgoGetParamType;

typedef enum
{
    AWBGetParamInputTypeInvalid             = -1,           ///< Invalid type
    AWBGetParamInputTypeAECData,                            ///< AEC input information
                                                            ///  Payload type: AWBAlgoExposureInformation
    AWBGetParamInputTypeFlashInformation,                   ///< Flash information for AWB
                                                            ///  Payload type: AWBAlgoFlashInformation
    AWBGetParamInputTypeCameraInfo,                         ///< Camera information for AWB
                                                            ///  Payload type: StatCameraInfo
    AWBGetParamInputTypeCCT,                                ///< CCT value passed to AWB to get gains
    AWBGetParamInputTypeGain,                               ///< AWB Gains passed to AWB to get CCT
    AWBGetParamInputTypeLastIndex,                          ///< Last valid index of the enum
                                                            ///  Payload type: None
    AWBGetParamInputTypeMax                 = 0x7FFFFFFF,   ///< Max type
} AWBAlgoGetParamInputType;

/// @brief Represents the input data to query which contains any associated data necessary to support that query
typedef struct
{
    VOID*                       pGetParamInput;         ///< Pointer to input data given in the query information to generate
                                                        ///  the output. See AWBAlgoGetParamInputType for details.
    UINT32                      sizeOfGetParamInput;    ///< Size of input payload
    AWBAlgoGetParamInputType    getParamInputType;      ///< Type of the payload
} AWBAlgoGetParamInput;

/// @brief Represents AWB module inputs to the AWB algorithm
typedef struct
{
    AWBAlgoGetParamInput*   pGetParamInputs;    ///< Pointer to AWBAlgoGetParamInput array
    UINT32                  getParamInputCount; ///< Number of inputs
} AWBAlgoGetParamInputList;

/// @brief Represents AWB output type
typedef enum
{
    AWBGetParamOutputTypeInvalid                    = -1,           ///< Invalid type
                                                                    ///  Payload: NULL & set size to zero
    AWBGetParamOutputTypeDebugData,                                 ///< Debug data used
                                                                    ///  Payload: StatsDataPointer
    AWBGetParamOutputTypeOutputList,                                ///< AWB output
                                                                    ///  Payload: AWBAlgoOutputList
    AWBGetParamOutputTypeAlgoVersion,                               ///< AWB algorithm version
                                                                    ///  Payload: StatsAlgorithmVersion
    AWBGetParamOutputTypeStatsRequired,                             ///< Informs if AWB algorithm requires or not stats.
                                                                    ///  Payload: BOOL
    AWBGetParamOutputTypeFlashEstimationProgress,                   ///< AWB flash estimation progress/status
                                                                    ///  Payload: AWBAlgoFlashEstimationProgress
    AWBGetParamOutputTypePublishingVendorTagsInfo,                  ///< Vendor tags from AWB algorithm
                                                                    ///  Payload: StatsVendorTagInfoList
    AWBGetParamOutputTypeDependentVendorTags,                       ///< Vendor tags from AWB algorithm
                                                                    ///  Payload: StatsVendorTagInfoList
    AWBGetParamOutputTypePeerInfo,                                  ///< Multi camera peer information
                                                                    ///  Payload: VOID*, pointing to algo internel peer
                                                                    ///  information data structure.
    AWBGetParamOutputTypeCCT,                                       ///< CCT Value from AWB algorithm
    AWBGetParamOutputTypeGains,                                     ///< AWB Gains from AWB algorithm
    AWBGetParamOutputTypeBGConfig,                                  ///< Get AWB stats config
                                                                    ///  Payload: StatsBayerGridBayerExposureConfig
    AWBGetParamOutputTypeLastIndex,                                 ///< Last valid index of the enum
                                                                    ///  Payload type: None
    AWBGetParamOutputTypeMax                        = 0x7FFFFFFF,   ///< Max type
} AWBAlgoGetParamOutputType;

/// @brief Represents AWB get output information from the algorithm
typedef struct
{
    VOID*                       pGetParamOutput;                ///< Pointer to the payload. See AWBAlgoGetParamType for details
                                                                ///  Algorithm implementer needs to do a deep copy to the given memory
    UINT32                      sizeOfGetParamOutput;           ///< Size of input payload pointed to by pGetParamOutput
    UINT32                      sizeOfWrittenGetParamOutput;    ///< Size of payload written back. If no output set to zero.
    AWBAlgoGetParamOutputType   getParamOutputType;             ///< Type of the payload
} AWBAlgoGetParamOutput;

/// @brief Represents AWB get output list from the algorithm
typedef struct
{
    AWBAlgoGetParamOutput*  pGetParamOutputs;       ///< Pointer to AWBAlgorithmOutput array.
    UINT32                  getParamOutputCount;    ///< Number of elements in pAWBOutputs
} AWBAlgoGetParamOutputList;

/// @brief Defines the AWB get parameter information
typedef struct
{
    AWBAlgoGetParamType         type;           ///< Type of the metadta query
    AWBAlgoGetParamInputList    inputInfoList;  ///< Data that's needed for processing the output
    AWBAlgoGetParamOutputList   outputInfoList; ///< Output requested in the query
} AWBAlgoGetParam;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describe AWB Algorithm Set Parameter definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Defines Illuminant factors
typedef struct
{
    BOOL    isAvailable;                ///< Indicates whether WB calibration data is avialabe or not.
    UINT32  correlatedColorTemperature; ///< Correlated Color Temperature: degrees Kelvin (K)
    FLOAT   ratioRG;                    ///< Red Green ratio
    FLOAT   ratioBG;                    ///< Blue Green ratio
} AWBAlgoIlluminatFactor;

/// @brief Defines Illuminants calibration factors
typedef struct
{
    UINT32                  numberOfIlluminants;                        ///< Number of illuminants calibrated
    AWBAlgoIlluminatFactor  illuminant[StatsIlluminantCount];           ///< Illuminants calibration factors
} AWBAlgoIlluminantsCalibrationFactor;

/// @brief Defines geometrical disparity calibration for dual camera
typedef struct
{
    UINT32  versionFormat;                                      ///< Calibration data format version.

    FLOAT   focalLengthMaster;                                  ///< Master camera focal length.
    FLOAT   focalLengthRatioMaster;                             ///< Master camera focal length ratio.

    FLOAT   focalLengthAuxiliary;                               ///< Auxiliary camera focal length.
    FLOAT   focalLengthRatioAuxiliar;                           ///< Auxiliar camera focal length ratio.

    INT32   sensorMirrorFlipSettingMaster;                      ///< Master sensor mirror and flip setting.
    INT32   sensorMirrorFlipSettingAuxiliar;                    ///< Auxiliary sensor mirror and flip setting.
    INT32   moduleOrientationFlag;                              ///< Sensor Module orientation
    INT32   rotationFlag;                                       ///< Sensor Module rotation

    FLOAT   relativePrinciplePointOffsetX;                      ///< Relative principle point offsetX
    FLOAT   relativePrinciplePointOffsetY;                      ///< Relative principle point offset Y
    INT32   relativePositionFlag;                               ///< Relative position flag
    FLOAT   relativeBaselineDistance;                           ///< Relative baseline distance
    UINT32  numberOfRelativeRotationMatrix;                     ///< Number of entries in relativeRotationMatrix
    FLOAT   relativeRotationMatrix[AWBAlgoRotationMatrixSize];  ///< Relative rotation Matix
} AWBAlgoGeometricalDisparityCalibration;

/// @brief This enum defines AWB modes
typedef enum
{
    AWBAlgoModeOff              = 0,            ///< Manual mode: using Color Correction Transform or gains.
    AWBAlgoModeAuto,                            ///< Auto White Balance: ON.
    AWBAlgoModeIncandescent,                    ///< Use preset values for illuminant A.
    AWBAlgoModeFluorescent,                     ///< Use preset values for illuminant F2.
    AWBAlgoModeWarmFluorescent,                 ///< Use preset values for illuminant F4.
    AWBAlgoModeDaylight,                        ///< Use preset values for illuminant D65.
    AWBAlgoModeCloudyDaylight,                  ///< Use preset values for a cloudy daylight light.
    AWBAlgoModeTwilight,                        ///< Use preset values for thilight light.
    AWBAlgoModeShade,                           ///< Use preset values for shade light.
    AWBAlgoModeMax              = 0x7FFFFFFF,   ///< Max type
} AWBAlgoMode;

/// @brief Defines AWB scene modes. These values are mirroring the APIs.
static const UINT32 AWBAlgoSceneModeDisabled            = 0;    ///< No scene mode is given.
static const UINT32 AWBAlgoSceneModeFacePriority        = 1;    ///< If face is detected, give priority while doing AWB.
static const UINT32 AWBAlgoSceneModeAction              = 2;    ///< Optimized for moving objects.
static const UINT32 AWBAlgoSceneModePortrait            = 3;    ///< Optimized for still photos of people.
static const UINT32 AWBAlgoSceneModeLandscape           = 4;    ///< Optimized for distant objects.
static const UINT32 AWBAlgoSceneModeNight               = 5;    ///< Optimized for low-light.
static const UINT32 AWBAlgoSceneModeNightPortrait       = 6;    ///< Optimized for still photos of people in low-light.
static const UINT32 AWBAlgoSceneModeTheatre             = 7;    ///< Optimized for dim, indoor settings where flash must be off.
static const UINT32 AWBAlgoSceneModeBeach               = 8;    ///< Optimized for beach scenes.
static const UINT32 AWBAlgoSceneModeSnow                = 9;    ///< Optimized for snow.
static const UINT32 AWBAlgoSceneModeSunset              = 10;   ///< Optimized for sunset.
static const UINT32 AWBAlgoSceneModeSteadyphoto         = 11;   ///< Optimized to avoid hand jitter.
static const UINT32 AWBAlgoSceneModeFireworks           = 12;   ///< Optimized for night fireworks.
static const UINT32 AWBAlgoSceneModeSports              = 13;   ///< Optimized for people moving fast.
static const UINT32 AWBAlgoSceneModeParty               = 14;   ///< Optimized for dim, indoor setting with people moving.
static const UINT32 AWBAlgoSceneModeCandlelight         = 15;   ///< Optimized for dim setting,with candles has the main source of light
static const UINT32 AWBAlgoSceneModeBarcode             = 16;   ///< Optimized for barcode pictures.
static const UINT32 AWBAlgoSceneModeBacklight           = 17;   ///< Optimized for scenes with backlight.
static const UINT32 AWBAlgoSceneModeFlowers             = 18;   ///< Optimized for flowers.
static const UINT32 AWBAlgoSceneModeAugmentedReality    = 19;   ///< Optimized for augmented reality.
static const UINT32 AWBAlgoSceneModeHDR                 = 20;   ///< Optimized for High Dynamic Range.
static const UINT32 AWBAlgoSceneModeDeviceCustomStart   = 100;  ///< Custom scene modes enum start, inclusive.

/// @brief Identifies manual AWB configuration
typedef struct
{
    AWBAlgoGains    gains;                        ///< AWB gains
    FLOAT           correlatedColorTemperature;   ///< CCT: Correlated Color Temperature
} AWBAlgoManualConfiguration;

/// @brief Flash type
typedef enum
{
    AWBAlgoFlashTypeLED,                    ///< Flash ilumination comes from LED
    AWBAlgoFlashTypeLCD,                    ///< Flash ilumination comes from LCD/screen
    AWBAlgoFlashTypeMax     = 0x7FFFFFFF,   ///< Max type
} AWBAlgoFlashType;

/// @brief Defines the AWB algorithm operation mode
typedef enum
{
    AWBAlgoOperationModeInit,                           ///< Indicate AWB in initial state
    AWBAlgoOperationModeStreaming,                      ///< Indicate AWB in streaming/preview state
    AWBAlgoOperationModePreflash,                       ///< Indicate AWB in preflash state
    AWBAlgoOperationModeFastConvergence,                ///< Indicate AWB in fast aec mode
    AWBAlgoOperationModeMax              = 0x7FFFFFFF   ///< Anchor to indicate the last item in the defines
} AWBAlgoOperationModeType;

/// @brief Identifies the type of input parameter to AWB algorithm
typedef enum
{
    AWBSetParamTypeInvalid                          = -1,           ///< Invalid type
    AWBSetParamTypeDebugDataMode,                                   ///< Sets the debug data mode
                                                                    ///  Payload type: INT32
    AWBSetParamTypeSensorInfo,                                      ///< Sensor info type
                                                                    ///  Payload type: StatsSensorInfo
    AWBSetParamTypeStatsWindow,                                     ///< Window information where statistics are been taken.
                                                                    ///  Payload: StatsWindowInfo
    AWBSetParamTypeChromatixData,                                   ///< Chromatix data type
                                                                    ///  Payload type: StatsTuningData
    AWBSetParamTypeIlluminantsCalibrationFactor,                    ///< Illuminants calibration
                                                                    ///  Payload type: AWBAlgoIlluminantsCalibrationFactor
    AWBSetParamTypeGeometricalDisparityCalibration,                 ///< Dual camara calibration for geometrical disparity
                                                                    ///  Payload type: AWBAlgoGeometricalDisparityCalibration
    AWBSetParamTypeLEDCalibrationData,                              ///< Factory calibration data done once LEDs are mounted on
                                                                    ///  device.
                                                                    ///  Payload type: StatsLEDCalibrationDataInput
    AWBSetParamTypeManualSettings,                                  ///< Set manual WB configuration, used in manual mode and to
                                                                    ///  set initial AWB settings before any stats is processed.
                                                                    ///  Payload type: AWBAlgoManualConfiguration
    AWBSetParamTypeWhiteBalanceMode,                                ///< Main switch for AWB, sets algo in auto, manual or
                                                                    ///  pre-defined illuminant target.
                                                                    ///  Payload type: AWBAlgoMode
    AWBSetParamTypeLock,                                            ///< Whether AWB is locked or not to the last calculated
                                                                    ///  value.
                                                                    ///  Payload type: BOOL
    AWBSetParamTypeSceneMode,                                       ///< Control which scene mode / best shot is active, if any.
                                                                    ///  Payload type: UINT32 (Please see the details under AWB
                                                                    ///  scene modes: "AWBAlgoSceneMode")
    AWBSetParamTypeFaceROI,                                         ///< Face detection algorithm provides face region of
                                                                    ///  interest.
                                                                    ///  Payload type: StatsFaceInformation
    AWBSetParamTypeTouchROI,                                        ///< Touch from UI provides the region of interest.
                                                                    ///  Payload type: StatsRectangle
    AWBSetParamTypeFlashType,                                       ///< Informs algorithm about the flash type. ex. LED, LCD.
                                                                    ///  Payload type: AWBAlgoFlashType
    AWBSetParamTypeFlashData,                                       ///< Information about the flash state and ratios.
                                                                    ///  Payload type: AWBAlgoFlashInformation
    AWBSetParamTypeDualcamSync,                                     ///< Information about dualcam sync.
                                                                    ///  Payload type: AWBAlgoDualcamSync
    AWBSetParamTypeCameraInfo,                                      ///< Information about camera id, role and type.
                                                                    ///  Payload type: StatsCameraInfo
    AWBSetParamTypeOperationMode,                                   ///< Information about Stats Processing Mode
                                                                    ///  Payload type: AWBAlgoOperationModeType
    AWBSetParamTypePeerInfo,                                        ///< Multi camera peer information
                                                                    ///  Payload type: VOID*
    AWBSetParamTypeWarmstart,                                        ///< Warmstart gains and CCT
                                                                    ///  Payload type: AWBAlgoWarmstartInformation
    AWBSetParamTypeLastIndex,                                       ///< Last valid index of the enum
                                                                    ///  Payload type: None
    AWBSetParamTypeMax                              = 0x7FFFFFFF,   ///< Max type
} AWBAlgoSetParamType;

/// @brief Represents an AWB input parameter
typedef struct
{
    const VOID*         pAWBSetParam;       ///< Pointer to AWB input param data
                                            ///  See AWBAlgoSetParamType for details
    UINT32              sizeOfInputParam;   ///< Size of data pointed to by pAWBInputParam
    AWBAlgoSetParamType setParamType;       ///< Type of the input parameter
} AWBAlgoSetParam;

/// @brief Represents an AWB input parameter passed to CHIAWBAlgorithm::AWBSetParam()
typedef struct
{
    AWBAlgoSetParam*    pAWBSetParams;      ///< Pointer to AWB input param data
                                            ///  See AWBAlgoSetParamType for details
    UINT32              inputParamsCount;   ///< Number of input param parameters
} AWBAlgoSetParamList;

/// @brief Identifies the type of create parameter to AWB algorithm
typedef enum
{
    AWBCreateParamsTypeInvalid              = -1,           ///< Invalid type
                                                            ///  Payload: NULL & set size to zero.
    AWBCreateParamsTypeTuningData,                          ///< Tuning data manager
                                                            ///  Payload type: StatsTuningData
    AWBCreateParamsTypeLoggerFunctionPtr,                   ///< Logging function pointer
                                                            ///  Payload type:  StatsLoggerFunction
    AWBCreateParamTypeOperationMode,                        ///< The AWB algorithm operation mode
                                                            ///  Payload type: AWBAlgoOperationModeType
    AWBCreateParamTypeSessionHandle,                        ///< Stats Session Handle
    AWBCreateParamTypeCameraOpenIndicator,                  ///< Actual camera open indicator
                                                            ///  Payload type:UINT 1=> actual camera open
    AWBCreateParamTypeCameraInfo,                           ///< Set camera information regarding algo role(master/slave)
                                                            ///  and camera ID during algorithm creation.
                                                            ///  Payload type: StatsCameraInfo
    AWBCreateParamsTypeCalibrationData,                     ///< Illuminant Calibration Factor
                                                            ///  Payload type IlluminantCalibrationFactor
    AWBCreateParamTypeCount,                                ///< Create Param Type Count
    AWBCreateParamsTypeMax                  = 0x7FFFFFFF    ///< Max type
} AWBAlgoCreateParamType;

/// @brief Represents AWB create parameter base structure, used as part of AWBAlgoCreateParamList
typedef struct
{
    VOID*                   pCreateParam;       ///< Payload: See AWBAlgoCreateParamType for details
    UINT32                  sizeOfCreateParam;  ///< Size of payload pointed by data.
    AWBAlgoCreateParamType  createParamType;    ///< Create parameter type.
} AWBAlgoCreateParam;

/// @brief Represents AWB create parameters information necessary to create AWB algorithm. See CreateAWB.
typedef struct
{
    AWBAlgoCreateParam* pCreateParams;        ///< A pointer to CreateAWB array
    UINT32              createParamsCount;    ///< Number of create parameters.
} AWBAlgoCreateParamList;

/// @brief Identifies the type of destroy parameter
typedef enum
{
    AWBDestroyParamTypeInvalid               = -1,          ///< Type Invalid
    AWBDestroyParamTypeCameraCloseIndicator,                ///< Camera Close Indicator
                                                            ///< UINT 1 actual Camera Close
    AWBDestroyParamTypeCameraInfo,                          ///< Camera info
                                                            ///  Payload: StatsCameraInfo
    AWBDestroyParamTypeCount,                               ///< Destroy Param Type Count
    AWBDestroyParamTypeMax                   = 0x7FFFFFFF   ///< Max Destroy Param Type
} AWBAlgoDestroyParamType;

/// @brief Represents an AWB destroy parameter
typedef struct
{
    AWBAlgoDestroyParamType destroyParamType;       ///< Type of parameter passed
    VOID*                   pParam;                 ///< Payload of the particular parameter type
    UINT32                  sizeOfParam;            ///< Size of the payload.
} AWBAlgoDestroyParam;

/// @brief Represents AWB algorithm destroy parameters
typedef struct
{
    AWBAlgoDestroyParam* pParamList;    ///< Pointer to AWB destroy-parameter
    UINT32               paramCount;    ///< Number of input destroy-parameters
} AWBAlgoDestroyParamList;

/// @brief Represents AWB algorithm feature capability
static const UINT64 AWBAlgoCapabilityAutoWB     = 0x1;          ///< Auto white balance capability
static const UINT64 AWBAlgoCapabilityManualWB   = (0x1 << 1);   ///< Maual white balance capability
static const UINT64 AWBAlgoCapabilitySingleLED  = (0x1 << 2);   ///< Single LED supported
static const UINT64 AWBAlgoCapabilityDualLED    = (0x1 << 3);   ///< Dual LED supported
static const UINT64 AWBAlgoCapabilitySceneMode  = (0x1 << 4);   ///< Scene mode supported

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Interface for AWB algorithm.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct CHIAWBAlgorithm
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AWBProcess
    ///
    /// @brief  Runs the AWB algorithm on the given inputs.
    ///
    /// @param  pCHIAWBAlgorithm   Pointer to CHIAWBAlgorithm instance
    /// @param  pInputs            Pointer to stats inputs interface
    /// @param  pOutputs           Pointer to stats data output by the algorithm
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*AWBProcess)(
        CHIAWBAlgorithm*           pCHIAWBAlgorithm,
        const AWBAlgoInputList*    pInputs,
        AWBAlgoOutputList*         pOutputs);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AWBGetParam
    ///
    /// @brief  This method retrieves the most up-to-date AWB algorithm information. The output
    ///         is generally updated after calling CHIAWBAlgorithm::Process()
    ///
    /// @param  pCHIAWBAlgorithm   Pointer to CHIAWBAlgorithm instance
    /// @param  pGetParam          Describes the metadata to query for, and contains any associated data necessary to support
    ///                            that query
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult(*AWBGetParam)(
        CHIAWBAlgorithm*    pCHIAWBAlgorithm,
        AWBAlgoGetParam*    pGetParam);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AWBSetParam
    ///
    /// @brief  Sets parameters to AWB.
    ///
    /// @param  pCHIAWBAlgorithm   Pointer to CHIAWBAlgorithm instance
    /// @param  pSetParams         AWB input parameters to set. Each element of the list is of type CHIAWBAlgorithmSetParams
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult(*AWBSetParam)(
        CHIAWBAlgorithm*             pCHIAWBAlgorithm,
        const AWBAlgoSetParamList*   pSetParams);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AWBGetFeatureCapability
    ///
    /// @brief  Sets parameters to AWB.
    ///
    /// @param  pCHIAWBAlgorithm   Pointer to CHIAWBAlgorithm instance
    /// @param  pFeatures          Features supported by the algorithm
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult(*AWBGetFeatureCapability)(
        CHIAWBAlgorithm*   pCHIAWBAlgorithm,
        UINT64*            pFeatures);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AWBDestroy
    ///
    /// @brief  This method destroys the derived instance of the interface
    ///
    /// @param  pCHIAWBAlgorithm    Pointer to CHIAWBAlgorithm instance
    /// @param  pDestroyParams      Pointer to destroy Param List
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID (*AWBDestroy)(
        CHIAWBAlgorithm*                 pCHIAWBAlgorithm,
        const AWBAlgoDestroyParamList*   pDestroyParams);
} CHIAWBAlgorithm; // CHIAWBAlgorithm interface

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateAWB
///
/// @brief  This method creates an instance to the CHIAWBAlgorithm.
///
/// @param  pCreateParams           Pointer to create params
/// @param  ppCHIAWBAlgorithm       Pointer to the created CHIAWBAlgorithm instance
///
/// @return CDKResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*CREATEAWB)(
    const AWBAlgoCreateParamList*   pCreateParams,
    CHIAWBAlgorithm**               ppCHIAWBAlgorithm);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHIAWBINTERFACE_H
