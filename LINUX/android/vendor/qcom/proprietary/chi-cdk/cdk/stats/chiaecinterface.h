////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chiaecinterface.h
/// @brief Auto Exposure Control Algorithm interface definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NOWHINE FILE GR016: Typedefs must not be used for structs, classes, and enumerations

#ifndef CHIAECINTERFACE_H
#define CHIAECINTERFACE_H

#include "chistatsinterfacedefs.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

static const INT    MaxSensorRegisterControlSize = 16;     ///< Max size of the sensor control registers
static const UINT32 MaxTableKnees                = 50;     ///< Defines Maximum number of knees allowed for a exposure table

/// Rules of input/output/setparam structures
/// 1. Every new variable has to be added to the end of the strcture once the interface is published
/// 2. No change to the sub-structures inside of the structures once published, if has to be changed, add the new variables to
///    the end of the master structure
///


/// @brief Defines the AEC algorithm running mode.
typedef enum
{
    AECAlgoModeAuto,                            ///< Auto mode
    AECAlgoModeManualISO,                       ///< ISO priority mode
    AECAlgoModeManualExpTime,                   ///< Exposure time priority mode
    AECAlgoModeManualFull,                      ///< Full manual mode
    AECAlgoModeMax             = 0x7FFFFFFF     ///< Anchor to indicate the last item in the defines
} AECAlgoModeType;

/// @brief Defines the AEC algorithm lock mode.
typedef enum
{
    AECAlgoLockOFF,                             ///< STATE: Lock AEC Settings OFF
    AECAlgoLockOn,                              ///< STATE: Lock AEC Settings ON
    AECAlgoLockMax              = 0x7FFFFFFF    ///< Anchor to indicate the last item in the defines
} AECAlgoLockType;

/// @brief Defines the different exposure parameter to apply to the frame
typedef enum
{
    AECAlgoExposureShort,                   ///< Represents the short exposure information
    AECAlgoExposureLong,                    ///< Represents the long exposure information
    AECAlgoExposureSafe,                    ///< Represents the safe exposure information. Safe means the exposure
                                            ///  value/sensitivity that results in a "well exposed" frame. Safe exposure in
                                            ///  Regular AEC: is simply the AEC output sensitivity
                                            ///  zzHDR: we will have short, long and safe(normal),
                                            ///  safe is the real correct exposure value
    AECAlgoExposureCount,                   ///< Represents the count of different exposure set
    AECAlgoExposureMax     = 0x7FFFFFFF     ///< Anchor to indicate the last item in the defines
} AECAlgoExposureType;

/// @brief Defines sHDR exposure numbers
typedef enum
{
    AECShdrSEF,                             ///< ShortExposureFrame
    AECShdrMEF,                             ///< SafeExposureFrame
    AECShdrLEF,                             ///< LongExposureFrame
    AECShdrExposureTotal,                   ///< sHDR Total exp numbers
} AECAlgoShdrExpsoures;

/// @brief Defines the AEC flash state
typedef enum
{
    AECAlgoFlashStateOff,                   ///< Flash is in off state
    AECAlgoFlashStatePre,                   ///< Flash is in preflash state
    AECAlgoFlashStateMain,                  ///< Flash in in mainflash state
    AECAlgoFlashStateCalibaration,          ///< Flash in in calibflash state
    AECAlgoFlashStateMax =   0x7FFFFFFF     ///< Anchor to indicate the last item in the defines
} AECAlgoFlashStateType;

/// @brief Defines the AEC flash measurements states
typedef enum
{
    AECAlgoFlashMeasKindOff,                ///< Flash measurement is not ongoing
    AECAlgoFlashMeasKindCalib,              ///< Flash is in preflash state
    AECAlgoFlashMeasKindTuning,             ///< Flash in in mainflash state
    AECAlgoFlashMeasKindMax = 0x7FFFFFFF    ///< Anchor to indicate the last item in the defines
} AECAlgoFlashMeasKindType;

/// @brief Defines the AEC flash measurements results
typedef enum
{
    AECAlgoFlashMeasStatusOngoing,          ///< Flash measurements still ongoing
    AECAlgoFlashMeasStatusPass,             ///< Flash measurements done with sucessful outcome
    AECAlgoFlashMeasStatusFail,             ///< Flash measurements done with failed outcome
    AECAlgoFlashMeasStatusBreak,            ///< Flash measurements done with tuning break
    AECAlgoFlashMeasStatusMax = 0x7FFFFFFF  ///< Anchor to indicate the last item in the defines
} AECAlgoFlashMeasStatusType;

/// @brief Defines the different type of snapshot
typedef enum
{
    AECAlgoSnapshotNormal,                  ///< The regular snapshot
    AECAlgoSnapshotFlash,                   ///< The flash snapshot
    AECAlgoSnapshotLLS,                     ///< The LLS snapshot: no flash, longer exposure time
    AECAlgoSnapshotCount,                   ///< Max number of snapshot Modes
    AECAlgoSnapshotMax     = 0x7FFFFFFF     ///< Anchor to indicate the last item in the defines
} AECAlgoSnapshotType;

/// @brief Defines the AEC state on touch EV event
typedef enum
{
    AECAlgoTouchEVInactive,                     ///< Touch EV feature is inactive
    AECAlgoTouchEVConverging,                   ///< Touch EV in searching state
    AECAlgoTouchEVSettled,                      ///< AEC is settled on touching EV
    AECAlgoTouchEVSceneChange,                  ///< Indicate the scene is changed after AEC is settled on touching
    AECAlgoTouchEVMax          = 0x7FFFFFFF     ///< Anchor to indicate the last item in the defines
} AECAlgoTouchEVType;

/// @brief Defines the AEC algorithm operation mode
typedef enum
{
    AECAlgoOperationModeInit,                           ///< Indicate AEC in initial state
    AECAlgoOperationModeStreaming,                      ///< Indicate AEC in streaming/preview state
    AECAlgoOperationModePreflash,                       ///< Indicate AEC in preflash state
    AECAlgoOperationModeFastAEC,                        ///< Indicate AEC in fast aec state
    AECAlgoOperationModeAggressiveAEC,                  ///< Indicate AEC in aggressive state
    AECAlgoOperationModeFlashMeasurement,               ///< Indicate AEC is measuring flash for either tuning or calibration
    AECAlgoOperationModeCount,                          ///< Max number of Operation Modes
    AECAlgoOperationModeMax            = 0x7FFFFFFF     ///< Anchor to indicate the last item in the defines
} AECAlgoOperationModeType;

/// @brief Defines the AEC metering mode
typedef enum
{
    AECAlgoMeteringModeFrameAverage,                    ///< Frame average
    AECAlgoMeteringModeCenterWeighted,                  ///< Center weighted
    AECAlgoMeteringModeSpot,                            ///< Spot metering
    AECAlgoMeteringModeMax             = 0x7FFFFFFF     ///< Anchor to indicate the last item in the defines
} AECAlgoMeteringModeType;

/// @brief Defines the LED mode
typedef enum
{
    AECAlgoLEDModeOff,                  ///< No LED for snapshot under any circumstance
    AECAlgoLEDModeAuto,                 ///< Let algorithm determine if need LED for snapshot
    AECAlgoLEDModeOn,                   ///< Turn on LED for snapshot under any circumstance
    AECAlgoLEDModeMax  = 0x7FFFFFFF     ///< Anchor to indicate the last item in the defines
} AECAlgoLEDModeType;

/// @brief Defines the Diagnostic Test Cases
typedef enum
{
    AECAlgoTestCaseTimeSweep,
    AECAlgoTestCaseGainSweep,
    AECAlgoTestCaseToggleTest,
    AECAlgoTestCaseExpTimeSweepDelay,
    AECAlgoTestCaseGainSweepDelay,
    AECAlgoTestCaseToggleTestDelay,
    AECAlgoTestCaseCount,
    AECAlgoTestCaseMax,
} AECAlgoDiagnosticTestCase;

/// @brief Defines the HDR types
typedef enum
{
    HDRDisabled,
    ISPHDR,
    SensorHDR,
}AECAlgoHDRType;

/// @brief Defines any manual override settings be set to the algorithm
typedef struct
{
    // All the override flags will be added here
    // These setting will be used to override tuning settings
    BOOL disableSpeckleDetection;      ///< Flag to enable/disable Speckle Dtection feature
    BOOL disablePreviewManualISO;      ///< Flag to enable/disable ISO Priority manual mode
    BOOL disablePreviewManualExpTime;  ///< Flag to enable/disable Exposure Time Priority manual mode
    BOOL disablePreviewManualFull;     ///< Flag to enable/disable Complete Manual Mode
    BOOL enableWarmStart;              ///< Flag to enable/disable AEC Warm Start
    UINT numSHDRFrames;                ///< Number of SHDR frames
} AECAlgoOverrideSettingParam;

/// @brief Represents AEC module EV settings capabilities
typedef struct
{
    INT8    minEV;      ///< min value of EV range capabilities
    INT8    maxEV;      ///< max value of EV range capabilities
    INT8    stepsPerEV; ///< Steps per EV should be configured in Chromatix and reported to HAL
} AECAlgoEVCapabilities;

/// @brief Defines the manual setting to AEC algorithm, supports ISO/Exposure time priority and full manual mode
typedef struct
{
    AECAlgoModeType manualMode;     ///< Specify the manual mode
    UINT32          ISOValue;       ///< Specify the ISO value used for manual mode
    UINT64          exposureTime;   ///< Specify the expoture time in nanoseconds used for manual mode
} AECAlgoManualSetting;

/// @brief Defines the maximum/minimum FPS for AEC algorithm
typedef struct
{
    FLOAT  maximumFPS;  ///< The maximum FPS AEC to operate at
    FLOAT  minimumFPS;  ///< The minimum FPS AEC to operate at
} AECAlgoFPSRange;

/// @brief Defines the AWB information as input to AEC algorithm
typedef struct
{
    BOOL    isAuto;     ///< Indicate if AWB runs at auto or manual mode
    FLOAT   redGain;    ///< Indicate the red gain
    FLOAT   greenGain;  ///< Indicate the green gain
    FLOAT   blueGain;   ///< Indicate the blue gain
    UINT32  CCT;        ///< CCT of awb
} AECAlgoWhiteBalanceState;

/// @brief Defines the Dual camera OTP calibration data
typedef struct
{
    UINT32  version;                ///< OTP data version info
    FLOAT   brightnessRatio;        ///< Brightness compensation ratio
    FLOAT   slaveGain;              ///< Other camera(slave) gain
    FLOAT   slaveTimeUnit;          ///< Other camera(slave) Time or linecount
    FLOAT   masterGain;             ///< Current camera(master) gain
    FLOAT   masterTimeUnit;         ///< Current camera(master) Time or linecount
    UINT32  CCT;                    ///< Current camera(master) calibration light CCT
} AECAlgoDCCalibrationInfo;

/// @brief Defines the exposure parameters to control the frame exposure setting to sensor and ISP
typedef struct
{
    UINT64  exposureTime;       ///< The exposure time applied to the frame, in nanoseconds
    FLOAT   linearGain;         ///< The total gain applied to the frame. Sensor should take this gain and apply as much of
                                ///  it as possible, and the remaining residual gain should be communicated to ISP to be
                                ///  applied
    FLOAT   sensitivity;        ///< The overall exposure, exposure time * linear gain * sensitivity correction factor
                                ///  (binning a.k.a.)
    FLOAT   deltaEVFromTarget;  ///< The current exposure delta compared with the desired target
} AECAlgoExposureData;

/// @brief Defines the exposure information structure includes the exposure data and the type of the exposure
typedef struct
{
    AECAlgoExposureType exposureType;  ///< The exposure type
    AECAlgoExposureData exposureData;  ///< The exposure data
} AECAlgoExposureInfo;

/// @brief Defines the input/output structures for the exposure parameters used between algorithm and camx
typedef struct
{
    AECAlgoExposureInfo*    pExposureInfo;              ///< The pointer to the array of AECAlgoExposureInfo
    UINT32                  exposureInfoCount;          ///< The count of AECAlgoExposureInfo in pExposureInfo. This is input
                                                        ///  to algorithm to indicate how many items in pExposureInfo.
    UINT32                  exposureInfoSize;           ///< The structure size of AECAlgoExposureInfo. This is input to
                                                        ///  algorithm to indicate each size of pExposureInfo.
    UINT32                  exposureInfoOutputCount;    ///< The output count of AECAlgoExposureInfo in pExposureInfo. Algorithm
                                                        ///  uses this field to indicate how many items was written.
    UINT32                  exposureInfoOutputSize;     ///< The output structure size of AECAlgoExposureInfo. Algorithm uses
                                                        ///  this field to indicate the size of memory was written for each item
} AECAlgoExposureSet;

/// @brief Defines the sensor information used by the AEC algorithm
typedef struct
{
    FLOAT   fNumber;                ///< Sensor Module F-number
    FLOAT   statsWindowAspectRatio; ///< Configured stats window Aspect Ratio (W / H)
    FLOAT   minimumStreamGain;      ///< Minimum total Gain for preview mode
    FLOAT   maximumStreamGain;      ///< Maximum total Gain for preview mode
    FLOAT   minimumStillGain;       ///< Minimum total Gain for snapshot mode
    FLOAT   maximumStillGain;       ///< Maximum total Gain for snapshot mode
    UINT64  minimumStreamExpTime;   ///< Minimum ExposureTime in nanoseconds for preview mode
    UINT64  maximumStreamExpTime;   ///< Maximum ExposureTime in nanoseconds  for preview mode
    UINT64  minimumStillExpTime;    ///< Minimum ExposureTime in nanoseconds  for snapshot mode
    UINT64  maximumStillExpTime;    ///< Maximum ExposureTime in nanoseconds  for snapshot mode
    UINT64 minSEFExpTime;
    UINT64 maxSEFExpTime;
    UINT64 minMEFExpTime;
    UINT64 maxMEFExpTime;
    UINT64 minLEFExpTime;
    UINT64 maxLEFExpTime;
} AECAlgoSensorInfo;

/// @brief Defines the Diagnostic configuration used by the AEC algorithm
typedef struct
{
    BOOL                               enable;                                  ///< Enable flag to conduct Diagnostic
    BOOL                               testCaseEnable[AECAlgoTestCaseCount];    ///< Test case enable flags
} AECAlgoDiagConfig;

/// @brief APEX values to provide basic parameters in logarithmic form
typedef struct
{
    FLOAT   brightnessValue;    ///< Brightness Value (BV)
    FLOAT   apertureValue;      ///< Aperture Value (AV)
    FLOAT   speedValue;         ///< Speed Value (SV)
    FLOAT   timeValue;          ///< Time Value (TV)
    FLOAT   exposureValue;      ///< Exposure Value (EV)
} AECAlgoAPEXData;

/// @brief Flash measurement configuration
typedef struct
{
    INT32   measurementsCount;      ///< How many measurements are required
    INT32   allowedReTriesCount;    ///< additional tries
    INT32   flashOnFrameCount;      ///< How many good frames required for measurement
} AECAlgoFlashMeasurementConfig;

typedef struct
{
    UINT32 count;
    FLOAT* pRGRatios;
    FLOAT* pBGRatios;
} AECAlgoFlashCalibrationData;

/// @brief Flash measurement
typedef struct
{
    FLOAT                       RGRatio;            ///< Flash measurements: red/green ratio
    FLOAT                       BGRatio;            ///< Flash measurements: blue/green ratio
    FLOAT                       flux;               ///< Flash measurements: flux
    AECAlgoFlashMeasStatusType  status;             ///< Flash measurements: status of this particular point
} FlashMeasurement;

/// @brief Flash measurement status
typedef struct
{
    AECAlgoFlashMeasStatusType  status;             ///< Ongoing, pass or failed
    INT32                       measurementCount;   ///< How many measurements were successfully performed
    FlashMeasurement*           pFlashPoints;       ///< Measurements: r/g and b/g ratios as well as flux
} AECAlgoFlashMeasurementResult;

/// @brief Y Histogram Stretch config data
typedef struct
{
    FLOAT                       clamp;              ///< Dark Clamp Value for Hist Stretch
    FLOAT                       scaling;            ///< Scaling Value for Hist Stretch
} AECAlgoYHistStretchData;

/// @brief Represents the frame control information after processing the stats
typedef struct
{
    AECAlgoFlashStateType       flashState;         ///< The flash state
    FLOAT                       LEDInfluenceRatio;  ///< The sensitivity ratio which is calculated from
                                                    ///  sensitivity with no flash / sensitivity with
                                                    ///  preflash
    FLOAT                       LEDRGRatio;         ///< The RG ratio when flash is on
    FLOAT                       LEDBGRatio;         ///< The BG ratio when flash is on
    FLOAT                       luxIndex;           ///< The lux index
    UINT32                      ISOValue;           ///< The ISO value
    AECAlgoYHistStretchData     stretchControl;     ///< The Histogram Stretch Control
    FLOAT                       LEDFirstEntryRatio; ///< ratio from first LED table entry
    FLOAT                       LEDLastEntryRatio;  ///< ratio from last LED table entry
    INT32                       IRModeActivated;    ///< IR LED Mode
    FLOAT                       IRLedIntensity;     ///< IR LED intensity
} AECAlgoFrameControl;

/// @brief The frame information output by AEC algorithm after processing the stats
typedef struct
{
    BOOL                    brightnessSettled;                  ///< Indicate if brightness is settled
    BOOL                    aecSettled;                         ///< Indicate if AEC is settled
    FLOAT                   asdExtremeGreenRatio;               ///< The extreme color green ratio
    FLOAT                   asdExtremeBlueRatio;                ///< The extreme color blue ratio
    AECAlgoSnapshotType     snapshotIndicator;                  ///< Indicate the snapshot type
    AECAlgoTouchEVType      touchEVIndicator;                   ///< Indicate the EV status
    BOOL                    LEDAFRequired;                      ///< Indicate if need LED to assist AF
    FLOAT                   legacyYStats[StatisticsYStatsNum];  ///< The Y(luma) stats of current frame
    BOOL                    LEDFDDelayRequired;                 ///< Indicate if delay is required when running FD in preflash
    SIZE_T                  DebugDataSizeWritten;               ///< Indicates the number of bytes written to debug data.
    FLOAT                   frameLuma;                          ///< Frame Luma
} AECAlgoFrameInfo;

/// @brief Defines the ROI to be used by the algorithm. The cooridinates will be normalized in range of [0..1]
typedef struct
{
    FLOAT   x;      ///< The x coordinate of the ROI, with value in range of [0..1]
    FLOAT   y;      ///< The y coordinate of the ROI, with value in range of [0..1]
    FLOAT   dx;     ///< The width of the ROI, with value in range of [0..1]
    FLOAT   dy;     ///< The height of the ROI, with value in range of [0..1]
    FLOAT   weight; ///< The weight of this ROI, with multiple ROIs set to algorithm, the larger weight value has more impact
                    ///  on metering result
} AECAlgoROI;

/// @brief Defines the input array of ROIs used by algorithm, including touch and face ROI
typedef struct
{
    AECAlgoROI* ROIs;       ///< The array of ROIs
    UINT32      ROICount;   ///< The total number of ROI in the array
} AECAlgoROIList;

/// @brief Defines the information associated with the stats sending to algorithm to process
typedef struct
{
    UINT64          requestId;              ///< The request id for the stats
    BOOL            prepareBeforePreflash;  ///< The flag of preparing preflash
    StatsDimension  statsWindowsSize;       ///< The dimension for the stats windows
} AECAlgoInputStatsInfo;

/// @brief Represents AEC output type
typedef enum
{
    AECAlgoOutputInvalid       = -1,        ///< Invalid type
    AECAlgoOutputFrameInfo     = 0,         ///< Frame information after processing stats
                                            ///  Payload AECAlgoFrameInfo
    AECAlgoOutputFrameControl,              ///< Frame control information after processing stats
                                            ///  Payload AECAlgoFrameControl
    AECAlgoOutputEXIFApex,                  ///< The APEX information for current frame
                                            ///  Payload AECAlgoAPEXData
    AECAlgoOutputLEDCurrents,               ///< The LED currents setting for preflash
                                            ///  Payload array of UINT32[StatisticsMaxNumOfLED]
    AECAlgoOutputExposureSets,              ///< The expsosure setting to sensor and ISP
                                            ///  Payload AECAlgoExposureSet
    AECAlgoOutputRealExposureSets,          ///< The expsosure setting to CHI for SHDR lib
                                            ///  Payload AECAlgoExposureSet
    AECAlgoOutputBGConfig,                  ///< The Bayer Grid configuration
                                            ///  Payload: StatsBayerGridBayerExposureConfig
    AECAlgoOutputAdditionalInfo,            ///< Additional output Info from algorithm
                                            ///  Payload: AECAlgoAdditionalInfo
    AECAlgoOutputVendorTag,                 ///< Vendor tag data. This is the list of vendor tags the algorithm wants
                                            ///  to publish.
                                            ///  Payload StatsVendorTagList
    AECAlgoOutputStretchControl,            ///< Histogram Stretch info
                                            ///  Payload AECAlgoYHistStretchData
    AECAlgoOutputBHistConfig,               ///< The Bayer HIstogram configuration
                                            ///  Payload: StatsBayerHistogramConfig
    AECAlgoOutputCount,                     ///< Indicate the total number of algorithm output
    AECAlgoOutputMax           = 0x7FFFFFFF ///< Anchor to indicate the last item in the defines
} AECAlgoOutputType;

///< @brief Defines the sensor register control structure
typedef struct
{
    INT32    registerControl[MaxSensorRegisterControlSize];        ///< Number of registers to control
} SensorRegisterControl;

/// @brief Defines the additional information which are output from the algorithm
typedef struct
{
    SensorRegisterControl    sensorControl;     ///< The sensor register control information from algorithm
} AECAlgoAdditionalInfo;

/// @brief Represents the one single type of AEC output from the algorithm
typedef struct
{
    VOID*               pAECOutput;             ///< Pointer to the payload. See AECAlgoOutputType for details
                                                ///  Algorithm implementer needs to do a deep copy
    UINT32              sizeOfAECOutput;        ///< Size of input payload pointed to by pAECOutput
    UINT32              sizeOfWrittenAECOutput; ///< Size of payload written back
    AECAlgoOutputType   outputType;             ///< Type of the payload
} AECAlgoOutput;

/// @brief Represents the array of AEC output from the algorithm
typedef struct
{
    AECAlgoOutput*  pAECOutputList;     ///< Pointer to AECAlgoOutput array.
    UINT32          numberOfAECOutputs; ///< Number of elements in pAECOutputList
} AECAlgoOutputList;


/// @brief Identifies the type of input parameter to AEC algorithm
typedef enum
{
    AECAlgoInputInvalid        = -1,            ///< Invalid type
    AECAlgoInputStatisticInfo  = 0,             ///< The information for the stats
                                                ///  Payload type: AECAlgoInputStatsInfo
    AECAlgoInputBayerGrid,                      ///< The Bayer stats
                                                ///  Payload type: StatsBayerGrid
    AECAlgoInputBayerHist,                      ///< The Bayer histogram
                                                ///  Payload type: StatsBayerHist
    AECAlgoInputIHist,                          ///< The Ihistogram
                                                ///  Payload type: StatsIHist
    AECAlgoInputVendorTag,                      ///< Vendor tag data. List of vendor tags the algorithm dependents on.
                                                ///  Payload type: StatsVendorTagList
    AECAlgoInputDebugData,                      ///< Debug data information. This is optional and can be NULL
                                                ///  Payload type: StatsDataPointer
    AECAlgoInputStatsChiHandle,                 ///< Algo needs to use this handle when querying vendor tags
                                                ///  Payload type: CHISTATSHANDLE
    AECAlgoInputHDRBHist,                       ///< The HDR Bayer histogram
                                                ///  Payload type: StatsBayerHist
    AECAlgoInputCameraInfo,                     ///< Set camera information regarding algo role(master/slave) and camera ID.
                                                ///  Payload type: StatsCameraInfo
    AECAlgoInputRDIStats,                       ///< The sensor RDI Stats
                                                ///  Payload type: HDR3Stats
    AECAlgoInputBayerHistNonUniform,            ///< The Non Uniform Bayer histogram
                                                ///  Payload type: StatsBayerHist
    AECAlgoInputBayerGridLEF,                   ///< The Bayer stats LEF Frame
                                                ///  Payload type: StatsBayerGrid
    AECAlgoInputBayerHistLEF,                   ///< The Bayer histogram LEF Frame
                                                ///  Payload type: StatsBayerHist
    AECAlgoInputHDRBHistLEF,                    ///< The HDR Bayer histogram LEF
                                                ///  Payload type: StatsBayerHist
    AECAlgoInputSHDRInterPolatedData,           ///< InterPolated Tuning Meta used in SHDR lib
                                                ///  Payload type: StatsSHDRParams
    AECAlgoInputCount,                          ///< Indicate the total number of algorithm input
    AECAlgoInputMax            = 0x7FFFFFFF     ///< Anchor to indicate the last item in the defines
} AECAlgoInputType;

/// @brief Represents one single AEC inputs to the AEC algorithm
typedef struct
{
    VOID*               pAECInput;      ///< Pointer to the payload. See AECAlgoInputType for details.
    UINT32              sizeOfAECInput; ///< Size of payload
    AECAlgoInputType    inputType;      ///< Type of the payload
} AECAlgoInput;

/// @brief Represents AEC module inputs to the AEC algorithm
typedef struct
{
    AECAlgoInput*   pAECInputList;      ///< Pointer to the list of payloads. See AECAlgoInput for details.
    UINT32          numberOfAECInputs;  ///< Number of discrete inputs
} AECAlgoInputList;

/// @brief Identifies the type of configuration to AEC algorithm
typedef enum
{
    AECAlgoSetParamInvalid                 = -1,        ///< Invalid type
    AECAlgoSetParamOperationMode,                       ///< The AEC algorithm operation mode
                                                        ///  Payload type: AECAlgoOperationModeType
    AECAlgoSetParamChromatixData,                       ///< The chromatix tuning header for AEC
                                                        ///  Payload type: StatsTuningData
    AECAlgoSetParamMeteringMode,                        ///< The metering mode
                                                        ///  Payload type: AECAlgoMeteringModeType
    AECAlgoSetParamExposureCompensation,                ///< Exposure compensation value
                                                        ///  Payload type: INT32
    AECAlgoSetParamFlickerCompensation,                 ///< The flicker compensation value
                                                        ///  Payload type: StatisticsAntibanding
    AECAlgoSetParamManualSetting,                       ///< The manual AEC setting
                                                        ///  Payload type: AECAlgoManualSetting
    AECAlgoSetParamAECLock,                             ///< The Exposure control lock setting
                                                        ///  Payload type: AECAlgoLockType
    AECAlgoSetParamFPSRange,                            ///< The FPS range
                                                        ///  Payload type: AECAlgoFPSRange
    AECAlgoSetParamLEDMode,                             ///< AEC LED mode
                                                        ///  Payload type: AECAlgoLEDModeType
    AECAlgoSetParamTouchROI,                            ///< The touch ROI set from hal/app
                                                        ///  Payload type: AECAlgoROIList
    AECAlgoSetParamFaceROI,                             ///< The face ROI from face detect module
                                                        ///  Payload type: AECAlgoROIList
    AECAlgoSetParamAWBState,                            ///< Current AWB state
                                                        ///  Payload type: AECAlgoWhiteBalanceState
    AECAlgoSetParamGyroInfo,                            ///< Current gyro information
                                                        ///  Payload type: StatsGyroInfo
    AECAlgoSetParamOverrideSetting,                     ///< Override Settings
                                                        ///  Payload type: AECAlgoOverrideSettingParam
    AECAlgoSetParamFlashMeasurement,                    ///< To Enable LED tuning or calibration mode
                                                        ///  Payload type: AECAlgoFlashMeasKindType
    AECAlgoSetParamFlashCalibrationData,                ///< Pass AEC Flash calibration data to AEC core
                                                        ///  Payload type: AECAlgoFlashCalibrationData
    AECAlgoSetParamDebugDataEnable,                     ///< To Enable the debug data
                                                        ///  Payload type: BOOL
    AECAlgoSetParamSensorInfo,                          ///< The sensor information
                                                        ///  Payload type: StatsSensorInfo
    AECAlgoSetParamCameraInfo,                          ///< Camera id, role and type Infomation
                                                        ///  Payload Type: StatsCameraInfo
    AECAlgoSetParamFlashMaximumCurrent,                 ///< The maximum available current could be used for flash shot
                                                        ///  Payload type: INT32
    AECAlgoSetParamSetDiagTestConfig,                   ///< The Diagnostic Configuration
                                                        ///  Payload type: AECAlgoDiagConfig
    AECAlgoSetParamStartUpExpSensitivity,               ///< The Sensitivity info for start up exposure override
                                                        ///  Payload Type : FLOAT[3]
    AECAlgoSetParamStatsCapability,                     ///< Sets the stats capabilities at initialization time
                                                        ///  Payload Type: StatsCapability
    AECAlgoSetParamCropWindow,                          ///< Sets the stats Crop window
                                                        ///  Payload Type: StatsRectangle
    AECAlgoSetParamPipelineDelay,                       ///< Set the pipeline delay
                                                        ///  Payload Type: UINT8
    AECAlgoSetParamPeerInfo,                            ///< Multi Camera Peer Information
                                                        ///  Payload Type: VOID Pointer
    AECAlgoSetParamDualcamCalibrationInfo,              ///< Dualcam calibration info
                                                        ///  Payload Type: AECAlgoDCCalibrationInfo
    AECAlgoSetParamMultiExpSensorMode,                  ///< Dualcam calibration info
                                                        ///  Payload Type: AECAlgoDCCalibrationInfo
    AECAlgoSetParamIsDualCamera,                        ///< Is dual camera or single
                                                        ///  Payload type: BOOL
    AECAlgoSetParamCustomExposureTable,                 ///< Custom exposure table read from vendor tag
                                                        ///  Payload type: AECCustomExposureTable
    AECAlgoSetParamDisableADRC,                         ///< Flag to enable ADRC
                                                        ///  Payload type: BOOL
    AECAlgoSetParamIRLedState,                          ///< IR LED State

    AECAlgoSetParamCompensationRatio,                   ///< Lux compensation Ratio

                                                        ///  Payload type: AECIRLed
    AECAlgoSetParamCount,                               ///< Indicate the total number of set param to algorithm
    AECAlgoSetParamMax                     = 0x7FFFFFFF ///< Anchor to indicate the last item in the defines
} AECAlgoSetParamType;

/// @brief Represents one single AEC configuration
typedef struct
{
    VOID*                   pAECSetParam;       ///< Pointer to AEC set param data
                                                ///  See AECAlgoSetParamType for details
    UINT32                  sizeOfAECSetParam;  ///< Size of data pointed to by pAECSetParam
    AECAlgoSetParamType     setParamType;       ///< Type of the input parameter
} AECAlgoSetParam;

/// @brief Represents the list of AEC configuration parameters passed to AECSetParam
typedef struct
{
    AECAlgoSetParam*    pAECSetParamList;       ///< Pointer to AEC set param data array
                                                ///  See AECAlgoSetParam for details
    UINT32              numberOfAECSetParams;   ///< Number of input param parameters
} AECAlgoSetParamList;

/// @brief Identifies the type of query to AEC algorithm
typedef enum
{
    AECAlgoGetParamInvalid             = -1,        ///< The invalid type
    AECAlgoGetParamStartExposure,                   ///< The exposure parameter on starting streaming
                                                    ///  Input payload: NULL
                                                    ///  Output payload: AECAlgoGetParamOutputList include following type
                                                    ///    AECAlgoGetParamOutputFrameControl,
                                                    ///    AECAlgoGetParamOutputExposureSet,
                                                    ///    AECAlgoGetParamOutputLEDCurrents,
                                                    ///    AECAlgoGetParamOutputEXIFApex
    AECAlgoGetParamSnapshotExposure,                ///< The exposure parameter for snapshot
                                                    ///  Input payload:  AECAlgoSnapshotConfig
                                                    ///  Output payload: AECAlgoGetParamOutputList include following type
                                                    ///    AECAlgoGetParamOutputFrameControl,
                                                    ///    AECAlgoGetParamOutputExposureSet,
                                                    ///    AECAlgoGetParamOutputLEDCurrents,
                                                    ///    AECAlgoGetParamOutputEXIFApex
    AECAlgoGetParamGainFromISOPreview,              ///< Calculate AEC gain from a given ISO for PREVIEW.
                                                    ///  Input payload: UINT32 ISOValue
                                                    ///  Output payload: AECAlgoGetParamOutputList include following type
                                                    ///    AECAlgoGetParamOutputGain
    AECAlgoGetParamGainFromISOSnapshot,             ///< Calculate AEC gain from a given ISO for SNAPSHOT.
                                                    ///  Input payload: UINT32 ISOValue
                                                    ///  Output payload: AECAlgoGetParamOutputList include following type
                                                    ///    AECAlgoGetParamOutputGain
    AECAlgoGetParamEVCapabilities,                  ///< Query EV capabilities from algorithm
                                                    ///  Input payload: NULL
                                                    ///  Output payload: AECAlgoEVCapabilities
    AECAlgoGetParamDebugData,                       ///< Queries the AEC debug data.
                                                    ///  Input payload: NULL
                                                    ///  Output payload: AECAlgoGetParamOutputList include following type
                                                    ///    AECAlgoGetParamOutputDebugData
    AECAlgoGetParamSWVersion,                       ///< Queries the SW version
                                                    ///  Input payload: NULL
                                                    ///  Output payload: AECAlgoGetParamOutputList include following type
                                                    ///    AECAlgoGetParamOutputSWVersion
    AECAlgoGetParamFlashMeasurementConfig,          ///< Queries flash measurement configuration for either tuning or calib
                                                    ///  Input payload: NULL
                                                    ///  Output payload: AECAlgoGetParamOutputList include following type
                                                    ///    AECAlgoGetParamOutputFlashMeasurementConfig
    AECAlgoGetParamFlashMeasurementResult,          ///< Queries flash measurement results for either tuning or calib
                                                    ///  Input payload: NULL
                                                    ///  Output payload: AECAlgoGetParamOutputList include following type
                                                    ///    AECAlgoGetParamOutputFlashMeasurementResult
    AECAlgoGetParamPublishingVendorTagsInfo,        ///< The vendor tags published by the algorithm
                                                    ///  Input payload: NULL
                                                    ///  Output payload: AECAlgoGetParamOutputList include following type
                                                    ///    AECAlgoGetParamOutputVendorTagInfoList
    AECAlgoGetParamDependentVendorTags,             ///< The vendor tags the algorithm depends on
                                                    ///  Input payload: NULL
                                                    ///  Output payload: AECAlgoGetParamOutputList include following type
                                                    ///    AECAlgoGetParamOutputVendorTagInfoList
    AECAlgoGetParamBGStatsConfig,                   ///< Bayer Grid statistics configuration
                                                    ///  Input payload: NULL
                                                    ///  Output payload: AECAlgoGetParamOutputList include following type
                                                    ///     AECAlgoGetParamOutputBGStatsConfig

    AECAlgoGetParamPeerInfo,                        ///< Multi Camera Peer Information
                                                    ///  Input payload: NULL
                                                    ///  Output payload: AECAlgoGetParamOutputList include following type
                                                    ///     AECAlgoGetParamOutputPeerInfo AECAlgoGetParamPeerInfo
    AECAlgoGetParamBHistStatsConfig,                ///< Bayer Histogram statistics configuration
                                                    ///  Input payload: NULL
                                                    ///  Output payload: AECAlgoGetParamOutputList include following type
                                                    ///     AECAlgoGetParamOutputBHistStatsConfig
    AECAlgoGetParamCount,                           ///< Indicate the total number of get param to algorithm
    AECAlgoGetParamMax                 = 0x7FFFFFFF ///< Anchor to indicate the last item in the defines
} AECAlgoGetParamType;

/// @brief Identifies the type of necessary input for AEC algorithm get query
typedef enum
{
    AECAlgoGetParamInputInvalid             = -1,        ///< The invalid type
    AECAlgoGetParamInputCameraInfo,                      ///< Camera id, role and type information pass to singleton algorithm
    AECAlgoGetParamInputSnapshotType,                    ///< Different type of snapshot
    AECAlgoGetParamInputISOValue,                        ///< ISOValue
    AECAlgoGetParamInputDebugDataPointer,                ///< Debug Data pointer
    AECAlgoGetParamInputCount,                           ///< Indicate the total number of get param to algorithm
    AECAlgoGetParamInputMax                 = 0x7FFFFFFF ///< Anchor to indicate the last item in the defines
} AECAlgoGetParamInputType;

/// @brief Defines the base structure for the get param output query
typedef enum
{
    AECAlgoGetParamOutputInvalid       = -1,            ///< The invalid type
    AECAlgoGetParamOutputFrameControl,                  ///< The framecontrol setting for the queried exposure
                                                        ///  Payload AECAlgoFrameControl
    AECAlgoGetParamOutputExposureSets,                  ///< The exposure settings for the queried exposure
                                                        ///  Payload AECAlgoExposureSet
    AECAlgoGetParamOutputLEDCurrents,                   ///< The LED current setting for the queried exposure
                                                        ///  Payload UINT32[StatisticsMaxNumOfLED]
    AECAlgoGetParamOutputEXIFApex,                      ///< The APEX information for the queried exposure
                                                        ///  Payload AECAlgoAPEXData
    AECAlgoGetParamOutputGain,                          ///< The gain value for the queried ISO value
                                                        ///  Payload FLOAT
    AECAlgoGetParamOutputFlashMeasurementConfig,        ///< Get flash calibration or tuning configuration
                                                        ///  Payload AECAlgoFlashMeasurementConfig
    AECAlgoGetParamOutputFlashMeasurementResult,        ///< Get flash measurement status
                                                        ///  Payload AECAlgoFlashMeasurementResult
    AECAlgoGetParamOutputDebugData,                     ///< The debug data from algorithm
                                                        ///  Payload StatsDataPointer
    AECAlgoGetParamOutputSWVersion,                     ///< The algorithm software version
                                                        ///  Payload StatsAlgorithmVersion
    AECAlgoGetParamOutputVendorTagInfoList,             ///< The vendor tag info list
                                                        ///  Payload StatsVendorTagInfoList
    AECAlgoGetParamOutputBGStatsConfig,                 ///< Bayer Grid Statistics configuration
                                                        ///  Payload StatsBayerGridBayerExposureConfig
    AECAlgoGetParamOutputEVCapabilities,                ///< Get EV Capabilities
                                                        ///  Payload AECAlgoEVCapabilities
    AECGetParamOutputTypeDependentVendorTags,           ///< The vendor tag info list
                                                        ///  Payload StatsVendorTagInfoList
    AECAlgoGetParamOutputPeerInfo,                      ///< The Multi Camera Peer Information
                                                        ///  Payload Pointer to Dual camera info.
    AECAlgoGetParamOutputBHistStatsConfig,              ///< Bayer Histogram Statistics configuration
                                                        ///  Payload StatsBayerGridBayerExposureConfig
    AECAlgoGetParamOutputCount,                         ///< The total count of GetParam type
    AECAlgoGetParamOutputMax           = 0x7FFFFFFF     ///< Anchor to indicate the last item in the defines
} AECAlgoGetParamOutputType;

/// @brief Represents the metadata to query for, and contains any associated data necessary to support that query
typedef struct
{
    AECAlgoGetParamInputType inputType;                 ///< Indicate the type of input data
    VOID*                    pInputData;                ///< Pointer to input data given in the query metadata
                                                        ///  to generate the output
    UINT32                   sizeOfInputData;           ///< Size of input data pointered to by pInputData
} AECAlgoGetParamInput;

/// @brief Represents the list of data necessary to support AEC query passed to AECGetParam
typedef struct
{
    AECAlgoGetParamInput*    pAECGetParamList;             ///< Pointer to AEC get param input array
                                                           ///  See AECAlgoGetParamInput for details
    UINT32                   numberOfAECGetInputParams;    ///< Number of input param parameters
} AECAlgoGetParamInputList;

/// @brief Represents the output for the metadata to query for
typedef struct
{
    VOID*                       pOutputData;                ///< Pointer to output data for which queried the metadata. See
                                                            ///  AECAlgoGetParamOutputType for details
    UINT32                      sizeOfOutputData;           ///< Size of output data pointed to by pOutputData
    UINT32                      sizeOfWrittenOutputData;    ///< Size of written output data pointed to by pOutputData
    AECAlgoGetParamOutputType   outputType;                 ///< Type of the payload
} AECAlgoGetParamOutput;

/// @brief Represents the array of AEC output from the algorithm
typedef struct
{
    AECAlgoGetParamOutput*  pAECOutputList;     ///< Pointer to AECAlgoGetParamOutput array.
    UINT32                  numberOfAECOutputs; ///< Number of elements in pAECGetParamOutputs
} AECAlgoGetParamOutputList;

/// @brief Represents the structure to query information from algorithm
typedef struct
{
    AECAlgoGetParamType        type;       ///< The type of information to get from algorithm
    AECAlgoGetParamInputList   inputList;  ///< The list of input used to indicate the detail information to get
    AECAlgoGetParamOutputList  outputList; ///< The list of output for queried information
} AECAlgoGetParam;

/// @brief Represents the different type of create parameters used to create the algorithm
typedef enum
{
    AECAlgoCreateParamsInvalid                   = -1,          ///< Invalid type
    AECAlgoCreateParamsLoggerFunctionPtr,                       ///< Logging function pointer
                                                                ///  Payload type: StatsLoggerFunction
    AECAlgoCreateParamTypeSessionHandle,                        ///< Stats Session Handle
    AECAlgoCreateParamTypeCameraOpenIndicator,                  ///< Camera Open Indicator
                                                                ///< UINT 1 actual camera open
    AECAlgoCreateParamTypeCameraInfo,                           ///< Set camera information regarding algo role(master/slave)
                                                                ///  and camera ID during algorithm creation.
                                                                ///  Payload type: StatsCameraInfo
    AECAlgoCreateParamTypeCount,                                ///< Create Param Type Count
    AECAlgoCreateParamsMax                      = 0x7FFFFFFF    ///< Anchor to indicate the last item in the defines
} AECAlgoCreateParamsType;

/// @brief Represents one single AEC configuration parameters used to create the AEC algorithm instance
typedef struct
{
    VOID*                   pCreateParam;       ///< Payload. See AECAlgoCreateParamType for details
    UINT32                  sizeOfCreateParam;  ///< Size of payload pointed by data.
    AECAlgoCreateParamsType createParamType;    ///< Create parameter type. See AECAlgoCreateParamType for details
} AECAlgoCreateParam;

/// @brief Represents AEC configuration parameters used to the AEC algorithm instance
typedef struct
{
    AECAlgoCreateParam* pCreateParamList;      ///< Array of creation parameters used to create the algorithm.
                                               ///  See AECAlgoCreateParam for details
    UINT32              numberOfCreateParams;  ///< Number of create parameters.
} AECAlgoCreateParamList;

/// @brief Identifies the type of destroy parameter
typedef enum
{
    AECDestroyParamTypeInvalid = -1,                                 ///< Type Invalid
    AECDestroyParamTypeCameraCloseIndicator,                         ///< Camera Close Indicator
                                                                     ///< UINT 1 actual Camera Close
    AECDestroyParamTypeCameraInfo,                                    ///< Camera info
                                                                     ///  Payload: StatsCameraInfo
    AECDestroyParamTypeCount,                                        ///< Destroy Param Type Count
    AECDestroyParamTypeMax     = 0x7FFFFFFF                          ///< Max Destroy Param Type
} AECAlgoDestroyParamType;

/// @brief Represents an AEC destroy parameter
typedef struct
{
    AECAlgoDestroyParamType destroyParamType;       ///< Type of parameter passed
    VOID*                   pParam;                 ///< Payload of the particular parameter type
    UINT32                  sizeOfParam;            ///< Size of the payload.
} AECAlgoDestroyParam;

/// @brief Represents AEC algorithm destroy parameters
typedef struct
{
    AECAlgoDestroyParam* pParamList;    ///< Pointer to AEC destroy-parameter
    UINT32               paramCount;    ///< Number of input destroy-parameters
} AECAlgoDestroyParamList;

/// @brief Represents the AEC features supported by the algorithm
typedef enum
{
    AECAlgoFeatureBasic        = 1 << 0,   ///< Basic AEC Feature - with basic functionalities, convergence, flash, manual aec etc
    AECAlgoFeatureLLS          = 1 << 1,   ///< Low Light non-flash snapshot/capture
    AECAlgoFeatureADRC         = 1 << 2,   ///< ADRC feature support
    AECAlgoFeatureZZHDR        = 1 << 3,   ///< ZzHDR feature support
    AECAlgoFeatureIHDR         = 1 << 4,   ///< iHDR feature support
    AECAlgoFeatureStaggeredHDR = 1 << 5,   ///< StaggeredHDR feature support
    AECAlgoFeatureFastAEC      = 1 << 6,   ///< Fast AEC required for initial faster ae convergence
    AECAlgoFeatureInstantAEC   = 1 << 7,   ///< Instant capture without preview
    AECAlgoFeatureTouchEV      = 1 << 8,   ///< Touch EV feature
    AECAlgoFeatureMultiCamera  = 1 << 9,   ///< Dual/Multi camera
    AECAlgoFeatureMax          = 1UL << 31,///< Anchor to indicate the last item in the defines
} AECAlgoFeatureType;

/// @brief Configures snapshot request
typedef struct
{
    AECAlgoSnapshotType         snapshotType;
    BOOL                        bUseAEBracketing;
    INT8                        EVVal;
} AECAlgoSnapshotConfig;

/// @brief Structure containing the Debug data pointer
typedef struct
{
    StatsDataPointer           debugData;
    SIZE_T                     sizeofDebugDataWritten;
} AECAlgoDebugDataConfig;

/// @brief AEC Startup mode
typedef enum
{
    AECAlgoStartupModeInvalid         = -1, ///< AEC Startup mode: Invalid
    AECAlgoStartupModeNormal          = 0,  ///< AEC Startup mode: Normal
    AECAlgoStartupModeFastConvergence = 1,  ///< AEC Startup mode: Fast Convergence
    AECAlgoStartupModeWarmStart       = 2   ///< AEC Startup mode: Warm Start
} AECAlgoStartupMode;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure containing previous session sensitivity information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    FLOAT              startUpIndices[ExposureIndexCount];         ///< AEC: Initial sensitivity information
    AECAlgoStartupMode startMode;                                  ///< AEC startup mode
} AECAlgoStartupIndices;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure containing previous session sensitivity information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    FLOAT              startUpSensitivity[ExposureIndexCount];     ///< AEC: Initial sensitivity information
    AECAlgoStartupMode startMode;                                  ///< AEC startup mode
} AECAlgoStartupSensitivity;

/// @brief Different IR LED States
typedef enum
{
    AECIRLedOFF   = 0, ///< IR LED OFF State
    AECIRLedON    = 1, ///< IR LED ON State
    AECIRLedAUTO  = 2  ///< IR LED AUTO State
} AECIRLedState;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure containing IR LED State and Status
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    AECIRLedState    state;    ///< IR LED state
    BOOL             ready;    ///< IR LED status true = ready or false = not ready
} AECIRLed;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure containing exposure table information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    BOOL                  isValid;                                     ///< Valid or Invalid status of exposure table
    FLOAT                 sensitivityCorrectionFactor;                 ///< Specify the sensitivity correction factor for
                                                                       ///  normalization between exposure tables
    UINT32                kneeCount;                                   ///< Number of knee entries in exposure table
    FLOAT                 gain[MaxTableKnees];                         ///< Gain for each knee point
    UINT64                expTime[MaxTableKnees];                      ///< Exposure time for each knee point
    INT32                 incrementPriority[MaxTableKnees];            ///< Specify exposure increment method for each knee point
    FLOAT                 expIndex[MaxTableKnees];                     ///< Exposure index for each knee point
    FLOAT                 antiBandingMinimumTimePercentageThreshold;   ///< Percentage of minimum exposure time the anti banding
                                                                       ///  is performed
} AECCustomExposureTable;

typedef struct
{
   BOOL  enable;
   UINT64 maxtime50Hz_L;
   UINT64 maxtime50Hz_M;
   UINT64 maxtime50Hz_S;
   UINT64 maxtime60Hz_L;
   UINT64 maxtime60Hz_M;
   UINT64 maxtime60Hz_S;
   UINT64 maxExpTime_L;
   UINT64 maxExpTime_M;
   UINT64 maxExpTime_S;
   UINT64 maxTotalGain_L;
   UINT64 maxTotalGain_M;
   UINT64 maxTotalGain_S;
   UINT64 minTotalGain_L;
   UINT64 minTotalGain_M;
   UINT64 minTotalGain_S;
   UINT64 minExpRatio;
} AECHDR3Values;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Interface for AEC algorithm.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct CHIAECAlgorithm
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AECProcess
    ///
    /// @brief  Runs the statistic algorithm on the given inputs. This method must only be called after the algorithm is
    ///         initialized.
    ///
    /// @param  pCHIAECAlgorithm   Pointer to CHIAECAlgorithm instance
    /// @param  pInput             Pointer to stats inputs interface
    /// @param  pOutput            Pointer to the stats data output by the algorithm
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*AECProcess)(
        CHIAECAlgorithm*        pCHIAECAlgorithm,
        const AECAlgoInputList* pInput,
        AECAlgoOutputList*      pOutput);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AECGetParam
    ///
    /// @brief  This method retrieves the most up-to-date AEC algorithm metadata. The output
    ///         is generally updated after calling CHIAECAlgorithm::AECProcess()
    ///
    /// @param  pCHIAECAlgorithm   Pointer to CHIAECAlgorithm instance
    /// @param  pGetParam          Describes the metadata to query for, and contains any associated data necessary to support
    ///                            that query
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*AECGetParam)(
        CHIAECAlgorithm*       pCHIAECAlgorithm,
        AECAlgoGetParam*       pGetParam);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AECSetParam
    ///
    /// @brief  Sets configuration parameters to AEC.
    ///
    /// @param  pCHIAECAlgorithm  Pointer to CHIAECAlgorithm instance
    /// @param  pSetParam         Stats configuration parameters to set.Each element of the list is of type AECAlgoInputParams
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*AECSetParam)(
        CHIAECAlgorithm*               pCHIAECAlgorithm,
        const AECAlgoSetParamList*     pSetParam);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AECGetFeatureCapability
    ///
    /// @brief  Gets the supported algorithm features.
    ///
    /// @param  pCHIAECAlgorithm  Pointer to CHIAECAlgorithm instance
    /// @param  pFeatures         The features returned from CHIAECAlgorithm, it will be bitmask of supported AECAlgoFeatureType
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*AECGetFeatureCapability)(
        CHIAECAlgorithm*   pCHIAECAlgorithm,
        UINT64*            pFeatures);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AECDestroy
    ///
    /// @brief  This method destroys the derived instance of the interface
    ///
    /// @param  pCHIAECAlgorithm   Pointer to CHIAECAlgorithm instance
    /// @param  pDestroyParams     Pointer to destroy Param List
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID (*AECDestroy)(
        CHIAECAlgorithm*                pCHIAECAlgorithm,
        const AECAlgoDestroyParamList*   pDestroyParams);

} CHIAECAlgorithm; // CHIAECAlgorithm interface

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateAEC
///
/// @brief  This function pointer to method of creating a handle of CHIAECAlgorithm.
///
/// @param  pCreateParam    Pointer to create param
/// @param  ppCHIAECAlgorithm  Pointer to the pointer of the created CHIAECAlgorithm instance
///
/// @return CDKResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*CREATEAEC)(
    const AECAlgoCreateParamList*      pCreateParam,
    CHIAECAlgorithm**                  ppCHIAECAlgorithm);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHIAECINTERFACE_H
