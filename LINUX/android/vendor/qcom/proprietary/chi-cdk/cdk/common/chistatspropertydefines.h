////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file chistatspropertydefines.h
/// @brief Define Qualcomm Technologies, Inc. stats proprietary data for defining internal properties/events
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHISTATSPROPERTYDEFINES_H
#define CHISTATSPROPERTYDEFINES_H

#include "camxcdktypes.h"

#ifdef __cplusplus

extern "C"
{
#endif // __cplusplus

/// @brief Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

static const UINT8  MaxCCMs              = 3;    ///< Max number of CCMs supported

// Common statistics static definitions
static const UINT8  ChannelIndexR        = 0;    ///< Index for R channel type
static const UINT8  ChannelIndexB        = 1;    ///< Index for B channel type
static const UINT8  ChannelIndexGR       = 2;    ///< Index for Gr channel type
static const UINT8  ChannelIndexGB       = 3;    ///< Index for Gb channel type

// Hardware stats capabilities supported, as used by StatsCapability::HWStatsCapabilities
static const UINT32 HWStatsCapabilityQuadSync           = (1 << 0); ///< Indicates whether the HW supports pixel quad
                                                                    ///  synchronization
static const UINT32 HWStatsCapabilitySaturationStats    = (1 << 1); ///< Indicates whether the HW supports saturation stats
static const UINT32 HWStatsCapabilityYStats             = (1 << 2); ///< Indicates whether the HW supports YStats
static const UINT32 HWStatsCapabilityMinLumaThreshold   = (1 << 3); ///< Indicates whether the HW supports setting minimum luma
                                                                    ///  thresholds

// Channel GR and GB are mutually exclusive with Y stats out mode used channel G and Y.
static const UINT8  ChannelIndexG        = 2;    ///< Index for G channel type
static const UINT8  ChannelIndexY        = 3;    ///< Index for Y channel type
static const UINT8  ChannelIndexCount    = 4;    ///< Maximum index for channel type
static const UINT32 RGBChannelCount      = 3;    ///< Total number of RGB channels

// AEC static definitions
static const UINT8  ExposureIndexShort    = 0;    ///< Exposure setting index for short exposure
static const UINT8  ExposureIndexLong     = 1;    ///< Exposure setting index for long exposure
static const UINT8  ExposureIndexSafe     = 2;    ///< Exposure setting index for safe exposure (between short and long)
static const UINT8  ExposureIndexCount    = 3;    ///< Exposure setting max index
static const UINT8  LEDSetting1           = 0;    ///< Index for LED setting 1
static const UINT8  LEDSetting2           = 1;    ///< Index for LED setting 2
static const UINT8  LEDSettingCount       = 2;    ///< Maximum number of LED Settings

// AWB static definitions
static const UINT8  AWBNumCCMRows               = 3;     ///< Number of rows for Color Correction Matrix
static const UINT8  AWBNumCCMCols               = 3;     ///< Number of columns for Color Correction Matrix
static const UINT8  AWBMaxCoefficientSize       = 6;     ///< Maximum coefficient size
static const UINT8  AWBMaxFIRCoefficientSize    = 11;    ///< Maximum FIR coefficient size

// AF static definitions
static const UINT8  MaxNumOfSteps             = 47;     ///< Maximum number of steps for lens move
static const UINT8  MaxHPFBufSize             = 22;     ///< Maximum size of high pass filter
static const UINT8  MaxBFGammaEntries         = 32;     ///< Maximum number of gamma entries
static const UINT8  MaxBFFilterCoringIndex    = 17;     ///< Maximum of rows for color color matrix
static const UINT8  MaxYConfig                = 3;      ///< Maximum array size of Y configuration
static const UINT8  MaxBFFIRFilterSize        = 13;     ///< Maximum FIR filter size
static const UINT32 BFMaxROIRegions           = 180;    ///< Maximum window configurations for BF stats

static const UINT8  FocusDistanceNearIndex       = 0;    ///< Index for near focus distance
static const UINT8  FocusDistanceOptimalIndex    = 1;    ///< Index for optimal focus distance
static const UINT8  FocusDistanceFarIndex        = 2;    ///< Index for far focus distance
static const UINT8  FocusDistanceMaxIndex        = 3;    ///< Maximum index for focus distance

static const UINT8  BFFilterTypeHorizontal1    = 0;    ///< Index for Horizontal 1 BF filter
static const UINT8  BFFilterTypeHorizontal2    = 1;    ///< Index for Horizontal 2 BF filter
static const UINT8  BFFilterTypeVertical       = 2;    ///< Index for Vertical BF filter
static const UINT8  BFFilterTypeCount          = 3;    ///< Maximum index of BF filter type

static const UINT32 MaxSoftwareFilterCoefficientsA      = 15;    /// Maximum number of A filter coefficients
static const UINT32 MaxSoftwareFilterCoefficientsB      = 15;    /// Maximum number of B filter coefficients
static const UINT32 MaxSoftwareFIRFilterCoefficients    = 15;    /// Maximum number of FIR filter coefficients

// ASD static declaration
static const UINT8  ASDPortrait     = 0;    ///< Index for Portrait mode
static const UINT8  ASDBacklight    = 1;    ///< Index for Backlight mode
static const UINT8  ASDLandscape    = 2;    ///< Index for Landscape mode
static const UINT8  ASDSnow         = 3;    ///< Index for Snow mode
static const UINT8  ASDHDR          = 4;    ///< Index for hdr mode
static const UINT8  ASDCount        = 5;    ///< Maximum index for scene detection type

// Face Detection static declaration
static const UINT8  MaxFaceROIs     = 10;   ///< Maximum index for Face ROIs

// PD static declaration
static const UINT8  PDMaxWindowSize = 200;    ///< Maximum size of PD output index

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes Common Statistics Data Structures
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Structure describing the rectangle coordinates
typedef struct
{
    UINT32    left;      ///< x coordinate of the ROI
    UINT32    top;       ///< y coordinate of the ROI
    UINT32    width;     ///< Width of the ROI
    UINT32    height;    ///< Height of the ROI
} RectangleCoordinate;

/// @brief Describes BGBEConfig output mode type
typedef enum BGBEOutputMode
{
    BGBERegular,              ///< BG or BE stats output regular mode
    BGBESaturationEnabled,    ///< BG or BE stats output satuation output enabled mode
    BGBEYStatsEnabled,        ///< BG or BE stats output Y stats output enabled mode
} BGBEOutputMode;

/// @brief Describes BGBEConfig output mode type
typedef enum GreenDataType
{
    Gr,              ///< Pick Gr data output for Y stats case
    Gb,              ///< Pick Gb data output for Y stats case
    GAverage,        ///< Pick average of Gr and Gb data for Y stats case
} GreenDataType;

/// @brief Structure describing BG and BE stats configuration
typedef struct
{
    UINT32                 horizontalNum;                              ///< Number of horizontal regions
    UINT32                 verticalNum;                                ///< Number of vertical regions
    RectangleCoordinate    ROI;                                        ///< ROI coordinates to configure
    UINT32                 channelGainThreshold[ChannelIndexCount];    ///< Max gain threshold for each channel
    UINT32                 outputBitDepth;                             ///< The output bit depths of the stats. This is an
                                                                       ///  option for stats algorithm (3rd party algos) to
                                                                       ///  specify that the stats that is passed to it
                                                                       ///  should have a certain bit depth.
    BGBEOutputMode         outputMode;                                 ///< Stats output mode
    FLOAT                  YStatsWeights[RGBChannelCount];             ///< YStats channel weights. Indexed by one of
                                                                       ///  ChannelIndexX values. Used when outputMode is
                                                                       ///  BGBEYStatsEnabled
    GreenDataType          greenType;                                  ///< Option to pick Gr, Gb, or G average (Gr + Gb >> 1)
    UINT32                 regionHeight;                               ///< Height of each stats subgrid region
    UINT32                 regionWidth;                                ///< Width of each stats subgrid region
    BOOL                   isStripeValid;                              ///< Incase of Dual IFE validate left or right stripe
                                                                       ///  based on striping library calculated output
} BGBEConfig;

/// @brief Enumeration class to describe what channel info is for which channel.
typedef enum
{
    ColorChannelInvalid,    ///< invalid channel type
    ColorChannelR,          ///< channel R
    ColorChannelGR,         ///< channel GR
    ColorChannelGB,         ///< channel GB
    ColorChannelB,          ///< channel B
    ColorChannelG,          ///< channel G
    ColorChannelY           ///< channel Y
} ColorChannel;

/// @brief Structure describing Bayer histogram configuration
typedef struct
{
    RectangleCoordinate    ROI;     ///< ROI rectangle to get statistics from
    ColorChannel           channel; ///< selected channel to collect
    BOOL                   uniform; ///< Uniform stats
} BHistConfig;

/// @brief HDRBHist options to select the input of the green channel
typedef enum IHistChannelSelection
{
    IHistYCCChannelY,   ///< channel Y
    IHistYCCChannelCb,  ///< channel Cb
    IHistYCCChannelCr,  ///< channel Cr
} IHistChannelSelection;

/// @brief Structure describing Image histogram configuration
typedef struct
{
    RectangleCoordinate     ROI;                ///< ROI rectangle to get statistics from
    IHistChannelSelection   channelYCC;         ///< YCC channel selection: 0: Luma (Y), 1: Chroma blue (Cb), 2: Chroma red (Cr)
    UINT32                  maxPixelSumPerBin;  ///< Max number histogram pixels sum to be represented per bin. Histogram pixels
                                                ///  are the pixels consider by the histogram. If image data gets concentrated
                                                ///  in few bins, output could get saturated. To help having more meaningful
                                                ///  values, and limit or avoid saturation, the output data is shifted, using
                                                ///  this parameter to decide the amount of bits to shift to fit into the output
                                                ///  bin size. Setting zero, means using default by IHist module.
} IHistStatsConfig;

/// @brief Structure describing CS stats configuration
typedef struct
{
    UINT32  statsHNum;                      ///< CS Config horizonal number
    UINT32  statsVNum;                      ///< CS Config vertical number
    UINT32  statsRgnWidth;                  ///< CS config region width
    UINT32  statsRgnHeight;                 ///< CS Config region height
    UINT32  statsHOffset;                   ///< CS config ROI horizontal offset
    UINT32  statsVOffset;                   ///< CS config ROI vertical offset
} CSStatsConfig;

/// @brief HDRBHist options to select the input of the green channel
typedef enum HDRBHistGreenChannelSelection
{
    HDRBHistSelectGR, ///< channel GR
    HDRBHistSelectGB, ///< channel GB
} HDRBHistGreenChannelSelection;

/// @brief HDRBHist selection of input pixels
typedef enum HDRBHistInputFieldSelect
{
    HDRBHistInputAll,           ///< All lines, to be use in non-HDR mode
    HDRBHistInputLongExposure,  ///< HDR mode: T1, long exposure pixels
    HDRBHistInputShortExposure, ///< HDR mode: T2, short exposure pixels
} HDRBHistInputFieldSelect;

/// @brief HDRBHist structure describing HDR Bayer histogram configuration
typedef struct
{
    RectangleCoordinate             ROI;                ///< ROI rectangle to get statistics from
    HDRBHistGreenChannelSelection   greenChannelInput;  ///< Selected input of the green channel
    HDRBHistInputFieldSelect        inputFieldSelect;   ///< Input Field selection, non-HDR(All) and HDR(T1 / T2) mode
} HDRBHistConfig;

/// @brief Structure describing the frame rate range
typedef struct
{
    INT32    minimum;   ///< The minimum FPS value; units are frames per second
    INT32    maximum;   ///< The maximum FPS value; units are frames per second
} FPSRange;

/// @brief Structure describing the Debug Data
typedef struct
{
    SIZE_T  size;   ///< Size in bytes of the debug data buffer
    VOID*   pData;  ///< Pointer to the debug data buffer
} DebugData;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes AEC Output Data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Preflash states as decided by AEC algorithm
typedef enum
{
    PreFlashStateInactive,        ///< Pre-flash off/inactive
    PreFlashStateStart,           ///< Estimation Starts; Preflash turns on - Sensor Node
    PreFlashStateTriggerFD,       ///< Start FD under preflash, consumed by FD Node
    PreFlashStateTriggerAF,       ///< Start AF under preflash, consumed by AF Node
    PreFlashStateTriggerAWB,      ///< Start AWB under preflash, consumed by AWB Node
    PreFlashStateCompleteLED,     ///< Estimation Completes with LED; Preflash turns Off - Sensor Node
    PreFlashStateCompleteNoLED,   ///< Estimation Completes but no LED required; - Sensor Node
    PreFlashStateRER              ///< Red Eye Reduction has started
} PreFlashState;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes AEC Output Data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Calibaration flash states as decided by AEC algorithm
typedef enum
{
    CalibrationFlashReady,                ///< Calib-flash off/inactive
    CalibrationFlashCollecting,           ///< Estimation Starts; Calibflash turns on - Sensor Node
    CalibrationPartialComplete,           ///< One of the measurement done; Calibflash turns off - Sensor Node
    CalibrationFlashComplete              ///< Estimation completes; Calibflash turns off - Sensor Node
} CalibrationFlashState;

/// @brief Defines the different type of snapshot
typedef enum
{
    SnapshotTypeNormal,    ///< The regular snapshot
    SnapshotTypeFlash,     ///< The flash snapshot
    SnapshotTypeLLS        ///< The LLS snapshot
} SnapshotTypes;

/// @brief Defines the AEC state on touch EV event
typedef enum
{
    AECTouchEVTypeInactive,      ///< Touch EV feature is inactive
    AECTouchEVTypeConverging,    ///< Touch EV in searching state
    AECTouchEVTypeSettled,       ///< AEC is settled on touching EV
    AECTouchEVTypeSceneChange    ///< Indicate the scene is changed after AEC is settled on touching
} AECTouchEVType;

/// @brief Precapture trigger states as decided by the AEC algorithm
typedef enum
{
    PrecapTriggerIdle,     ///< The trigger is idle
    PrecapTriggerStart,    ///< The precapture metering sequence will be started by the camera device
    PrecapTriggerCancel    ///< The camera device will cancel any currently active or computed precapture metering sequence
} PrecapTriggers;

/// @brief Structure describing the AEC weighted regions
typedef struct
{
    INT32  left;      ///< Left coordinate of the region
    INT32  top;       ///< Top coordinate of the region
    INT32  width;     ///< Width of the region, with left + width - 1 pointing to the right-most pixel in the region
    INT32  height;    ///< Height of the region with top + height - 1 pointing to the bottom-most pixel in the region
    INT32  weight;    ///< Region weight. Value must be between [0, 1000], with region weight of 0 being ignored
} AECRegion;

/// @brief Structure describing top-level AEC stat configuration
typedef struct
{
    BGBEConfig      BGConfig;         ///< BG configuration
    BGBEConfig      BEConfig;         ///< BE configuration
    BHistConfig     BHistConfig;      ///< BHist configuration
    HDRBHistConfig  HDRBHistConfig;   ///< HDR BHist configuration
    BGBEConfig      TintlessBGConfig; ///< Tintless BG Configuration
} AECConfig;

/// @brief Defines the exposure parameters to control the frame exposure setting to sensor and ISP
typedef struct
{
    UINT64  exposureTime;         ///< The exposure time in nanoseconds used by sensor
    FLOAT   linearGain;           ///< The total gain consumed by sensor only
    FLOAT   sensitivity;          ///< The total exposure including exposure time and gain
    FLOAT   deltaEVFromTarget;    ///< The current exposure delta compared with the desired target
} AECExposureData;

/// @brief Structure describing AEC output data for the current frame
typedef struct
{
    // Internal Member Variables
    PrecapTriggers  AECPrecaptureTrigger;                ///< AEC Precapture trigger; consumed by HAL
    PreFlashState   AECPreFlashState;                    ///< Preflash state from AEC
    BOOL            AECSettled;                          ///< Indicate if AEC is settled
    VOID*           pCustomData;                         ///< Custom data passed from algorithm
    VOID*           pDebugData;                          ///< Debug data passed from algorithm
    AECExposureData exposureInfo[ExposureIndexCount];    ///< Exposure info consumed by sensor, ISP, and HAL
    UINT32          ISOValue;                            ///< The ISO value
    BOOL            LEDAFRequired;                       ///< Indicate if need LED to assist AF
    UINT32          LEDCurrents[LEDSettingCount];        ///< LED currents for flash driver
    FLOAT           luxIndex;                            ///< The lux index for ISP
    SnapshotTypes   snapshotIndicator;                   ///< Indicate the snapshot type
    AECTouchEVType  touchEVIndicator;                    ///< Indicate the EV status
    UINT64          frameDuration;                       ///< Frame Duration
    // Fields mainly required for EXIF here
    UINT16          meteringMode;                        ///< Metering Mode
    UINT16          exposureProgram;                     ///< Exposure Program
    FLOAT           brightnessValue;                     ///< Brightness value
    UINT16          sceneType;                           ///< Scene Type
    UINT16          exposureMode;                        ///< Exposure modes set
    FLOAT           frameLuma;                           ///< Frame Luma
} AECFrameInformation;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes AWB Output Data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Structure describing AWB gains
typedef struct
{
    FLOAT   rGain;    ///< Red gains
    FLOAT   gGain;    ///< Green gains
    FLOAT   bGain;    ///< Blue gains
} AWBGainParams;

/// @brief Structure describing AWB BG and BE stats
typedef struct
{
    BGBEConfig    BGConfig;    ///< BG configuration parameters
} AWBConfig;

///< @brief Defines the format of the color correction matrix
typedef struct
{
    BOOL        isCCMOverrideEnabled;               ///< Flag indicates if CCM override is enabled.
    FLOAT       CCM[AWBNumCCMRows][AWBNumCCMCols];  ///< The color correction matrix
    FLOAT       CCMOffset[AWBNumCCMRows];           ///< The offsets for color correction matrix
}AWBCCMParams;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes Auto White Balance (AWB) Frame Information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    AWBGainParams    AWBGains;            ///< R/G/B gains
    UINT32           colorTemperature;    ///< Color temperature
} AWBFrameInfo;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes AF Output Data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///@brief Enumeration describing Auto Focus status
typedef enum
{
    AFStatusInvalid     = -1,           ///< Auto focus is in error state
    AFStatusInitialized = 0,            ///< Auto focus is in initialized state
    AFStatusFocusing,                   ///< Auto focus is currently focusing
    AFStatusFocused,                    ///< Auto focus is in focused state
    AFStatusNotFocused,                 ///< Auto focus is not in focused state
    AFStatusTypeMax     = 0x7FFFFFFF    ///< Type max
} AFStatus;

/// @brief Structure describing AF status
typedef struct
{
    BOOL        focusDone;                               ///< Focus is done flag
    AFStatus    status;                                  ///< Current focusing status
    FLOAT       focusDistance[FocusDistanceMaxIndex];    ///< Distances where an object appears to be in focus(in meters)
} AFStatusParams;

/// @brief Enumeration describing BF stats channel gains.
typedef enum
{
    BFChannelInvalid    = -1,            ///< Type Invalid
    BFChannelSelectG,                    ///< G channel type
    BFChannelSelectY,                    ///< Y channel type
    BFChannelSelectMax  = 0x7FFFFFFF,    ///< Max channel type
} BFChannelSelectType;

/// @brief Enumeration describing input channel type.
typedef enum
{
    BFInputInvalid      = -1,            ///< Type Invalid
    BFInputSelectGr,                     ///< Select GR channel for BF
    BFInputSelectGb,                     ///< Select GB channel for BF
    BFInputSelectMax    = 0x7FFFFFFF,    ///< Max channel type
} BFInputGSelectType;

/// @brief Structure describing input config parameters.
typedef struct
{
    BOOL                isValid;                 ///< Is input config valid
    BFChannelSelectType BFChannelSel;            ///< Channel selection config
    BFInputGSelectType  BFInputGSel;             ///< G channel selection config
    FLOAT               YAConfig[MaxYConfig];    ///< Y channel config
} BFInputConfigParams;

/// @brief Structure describing look up table config
typedef struct
{
    BOOL    isValid;                        ///< Is Gamma LUT valid
    UINT32  gammaLUT[MaxBFGammaEntries];    ///< Gamma Look up table
    UINT32  numGammaLUT;                    ///< Number of entries in the Gamma LUT
} BFGammaLUTConfigType;

/// @brief Structure describing BF scale configuration.
typedef struct
{
    BOOL    isValid;                 ///< Is BF scale config valid
    BOOL    BFScaleEnable;           ///< Flag to enable BF scalar
    INT32   scaleM;                  ///< M scalar value
    INT32   scaleN;                  ///< N scalar value
    INT32   pixelOffset;             ///< Pixel Offset
    INT32   mnInit;                  ///< mnInit
    INT32   interpolationResolution; ///< Interpolation resolution
    INT32   phaseInit;               ///< Phase Init
    INT32   phaseStep;               ///< Phase step
    INT32   inputImageWidth;         ///< Input Image Width
} BFScaleConfigType;

/// @brief Structure describing FIR filter configuration
typedef struct
{
    BOOL     enable;                                    ///< FIR filter config enable
    INT32    FIRFilterCoefficients[MaxBFFIRFilterSize]; ///< FIR filter coefficient
    UINT32   numOfFIRCoefficients;                      ///< Number of filter coefficient
} BFFIRFilterConfigType;

/// @brief Structure describing IIR filter configuration
typedef struct
{
    BOOL     enable;    ///< IIR filter enable
    FLOAT    b10;       ///< IIR filter coefficient
    FLOAT    b11;       ///< IIR filter coefficient
    FLOAT    b12;       ///< IIR filter coefficient
    FLOAT    a11;       ///< IIR filter coefficient
    FLOAT    a12;       ///< IIR filter coefficient
    FLOAT    b20;       ///< IIR filter coefficient
    FLOAT    b21;       ///< IIR filter coefficient
    FLOAT    b22;       ///< IIR filter coefficient
    FLOAT    a21;       ///< IIR filter coefficient
    FLOAT    a22;       ///< IIR filter coefficient
} BFIIRFilterConfigType;

/// @brief Enumeration describing configuration window type
typedef enum
{
    BFStatsInvalidRegion    = -1,            ///< Type Invalid
    BFStatsPrimaryRegion,                    ///< BF stats primary region type
    BFStatsSecondaryRegion,                  ///< BF stats secondary region type
    BFStatsMaxRegion        = 0x7FFFFFFF,    ///< BF stats max region type
} BFStatsRegionType;

/// @brief Structure describing region of interest type
typedef enum
{
    BFStatsInvalidROI    = -1,          ///<  Type Invalid
    BFStatsDefaultROI,                  ///< BF stats default ROI type
    BFStatsCustomROI,                   ///< BF stats custom ROI type
    BFStatsMaxROI        = 0x7FFFFFFF   ///< BF stats max ROI type
} BFStatsROIType;

/// @brief Structure describing BF ROI dimension
typedef struct
{
    BFStatsRegionType   region;       ///< BF region type
    RectangleCoordinate ROI;          ///< ROI coordinates to configure
    UINT32              regionNum;    ///< Number of regions
    BOOL                isValid;      ///< Is ROI valid
} BFStatsROIDimensionParams;

/// @brief Structure describing BF ROI configuration
typedef struct
{

    BFStatsROIDimensionParams  BFStatsROIDimension[BFMaxROIRegions];    ///< BF ROI configuration
    UINT32                     numBFStatsROIDimension;                  ///< Number of ROI regions
    BFStatsROIType             BFROIType;                               ///< ROI type
    UINT32                     lastPrimaryRegion;                       ///< Last primary region
} BFStatsROIConfigType;

/// @brief Structure describing Core filter configuration
typedef struct
{
    INT32     threshold;                       ///< BF filter threshold
    UINT32    core[MaxBFFilterCoringIndex];    ///< BF filter cores
    UINT32    gain;                            ///< BF filter gain
} BFFilterCoringConfigParams;

/// @brief Structure describing BF filter configuration
typedef struct
{
    BOOL                          isValid;                  ///< Is Filter config valid
    BOOL                          horizontalScaleEnable;    ///< Is horizontal scale enabled
    BFFIRFilterConfigType         BFFIRFilterConfig;        ///< FIR filter configuration
    BFIIRFilterConfigType         BFIIRFilterConfig;        ///< IIR filter configuration
    INT32                         shiftBits;                ///< Number of bits to shift
    BFFilterCoringConfigParams    BFFilterCoringConfig;     ///< BF coring configuration
} BFFilterConfigParams;

/// @brief Structure describing FloatingWindow config parameters
typedef struct
{
    BFInputConfigParams     BFInputConfig;                        ///< BF input configuration
    BFGammaLUTConfigType    BFGammaLUTConfig;                     ///< BF Gamma LUT configuration
    BFScaleConfigType       BFScaleConfig;                        ///< BF Scale configuration
    BFFilterConfigParams    BFFilterConfig[BFFilterTypeCount];    ///< BF filter configuration
    BFStatsROIConfigType    BFStatsROIConfig;                     ///< BF ROI configuration
} BFStatsConfigParams;

/// @brief Enumeration describing BF stat type
typedef enum
{
    BFStats = (1 << 0),    ///< Enumeration for Single BF
    AFMax   = (1 << 1),    ///< Enumertion for Max value of AF config enum
} AFConfigMask;

/// @brief Structure describing top level AF stats configuration
typedef struct
{
    AFConfigMask           mask;       ///< Stats config mask
    BFStatsConfigParams    BFStats;    ///< BF FloatingWindow stats config
} AFConfigParams;

/// @brief Structure describing PDAF window type
typedef enum
{
    DefaultWindow,      ///< Default window configuration
    FixedGridWindow,    ///< Fixed window configuration
    WindowModeMax,      ///< Max window configuration type
} PDAFSwWindowType;

/// @brief Structure describing Fixed window configuration
typedef struct
{
    UINT32                 horizontalNum;    ///< Number of horizontal regions
    UINT32                 verticalNum;      ///< Number of vertical regions
    RectangleCoordinate    AFFixedWindow;    ///< Window configuration
} FixedGridWindowParams;

/// @brief Structure describing top level PDAF ROI configuration
typedef struct
{
    PDAFSwWindowType         PDAFSwWindowMode;            ///< Default/fixed window mode
    FixedGridWindowParams    fixedGridWindowConfigure;    ///< Fixed grid configuration
} PDAFConfigureParams;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes Move Lens Output Data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Structure describing move lens data
typedef struct
{
    UINT32    targetLensPosition;       ///< Target lens position in step
    BOOL      useDACValue;              ///< use digital to analog convertor
    INT32     position[MaxNumOfSteps];  ///< DAC position values
    UINT8     delay[MaxNumOfSteps];     ///< DAC delay to be applied
    UINT8     numIntervals;             ///< Number of intervals
} MoveLensOutput;

/// @brief Structure describing FOV compensation factor
typedef struct
{
    FLOAT fieldOfViewCompensationFactor;
} FOVCOutput;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes Software Stat Configuration Filters Data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Structure describing Software Stat Configuration Filters
typedef struct
{
    BOOL                   enable;                                               ///< Enable/disable software stats
    RectangleCoordinate    ROI;                                                  ///< Region of interest
    UINT32                 numCoefficientsA;                                     ///< Number of A coefficients
    FLOAT                  coefficientsA[MaxSoftwareFilterCoefficientsA];        ///< A coefficient values
    FLOAT                  numCoefficientsB;                                     ///< Number of B Coefficients
    FLOAT                  coefficientsB[MaxSoftwareFilterCoefficientsB];        ///< B coefficient values
    FLOAT                  numCoefficientsFIR;                                   ///< Number of FIR Coefficients
    FLOAT                  coefficientsFIR[MaxSoftwareFIRFilterCoefficients];    ///< FIR Coefficients values
    DOUBLE                 minFocusValue;                                        ///< Minimum Focus Value
} SoftwareStatsFilterConfig;

/// @brief Structure describing AF external output data for frame N
typedef struct
{
    // Internal Member Variables
    AFStatusParams         focusStatus;                ///< Auto focus status data
    FLOAT                  focusValue;                 ///< Auto focus value
    BOOL                   spotLightDetected;          ///< Spot Light Detected
    FLOAT                  cropMagnificationFactor;    ///< Crop Magnification Factor
    RectangleCoordinate    ROICoordinate;              ///< ROI rectangle coordinates
    MoveLensOutput         moveLensOutput;             ///< Lens move information
    FOVCOutput             fovcOutput;                 ///< FOVC information
} AFFrameInformation;

/// @brief Structure describing AF output data
typedef struct
{
    AFStatusParams         status;         ///< Auto focus status data
    AFConfigParams         statsConfig;    ///< BF configuration data
    PDAFConfigureParams    PDAFConfig;     ///< PDAF configuration data
} AFOutput;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes PDLib Output Data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Structure describing PD output data unit
struct PDDefocusInfo
{
    UINT32  numberOfDefocus;                    ///< Number of defocus values
    INT32   defocus[PDMaxWindowSize];           ///< defocus value of the grid
    UINT32  confidenceLevel[PDMaxWindowSize];   ///< confidence level of pd grid. Range 0:1024
    FLOAT   phaseDifference[PDMaxWindowSize];   ///< phase difference of the grid
};

/// @brief Structure describing high level PD output data
struct PDOutput
{
    PDDefocusInfo   defocus;            ///< defocus data of main grids
    PDDefocusInfo   peripheralDefocus;  ///< defocus data of peripheral grids
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes ASD Output Data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Structure describing ASD output data
typedef struct
{
    UINT32    severity[ASDCount];    ///< Severity array from 0 - 255
    BOOL      detected[ASDCount];    ///< Detection array
} ASDOutput;

/// @brief JpegThumbnailSize
typedef struct
{
    UINT32 JpegThumbnailSize_0;
    UINT32 JpegThumbnailSize_1;
} JPEGThumbnailSize;

/// @brief Describes Face ROI
typedef struct
{
    UINT32                 id;           ///< Id used to track a face in the scene
    UINT32                 confidence;   ///< Confidence of this face
    RectangleCoordinate    faceRect;     ///< Detected Face rectangle
} FaceROIData;

/// @brief Structure describing Face ROI data
typedef struct
{
    UINT64         requestId;                       ///< Request ID corresponding to this results
    UINT32         ROICount;                        ///< Number of ROIs
    FaceROIData    unstabilizedROI[MaxFaceROIs];    ///< Unstabilized Face ROI data
    FaceROIData    stabilizedROI[MaxFaceROIs];      ///< Stabilized Face ROI data
} FaceROIInformation;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes AFD stats rs config
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Structure describing AFD stats rs config
typedef struct
{
    UINT32  statsHNum;                      ///< RS Config horizonal number
    UINT32  statsVNum;                      ///< RS Config vertical number
    UINT32  statsRgnWidth;                  ///< RS config region width
    UINT32  statsRgnHeight;                 ///< RS Config region height
    UINT32  statsHOffset;                   ///< RS config ROI horizontal offset
    UINT32  statsVOffset;                   ///< RS config ROI vertical offset
    BOOL    statsRSCSColorConversionEnable; ///< RS/CS Input color format, enable conversion from RGB to Y
} AFDStatsRSConfig;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes AFD frame information Data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Structure describing AFD frame information data
typedef struct
{
    // Internal Member Variables
    UINT32    detectedAFDMode;    ///< Detected anti banding mode
} AFDFrameInfo;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes AFD frame information Data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma pack(pop) // CDK_PACK_SIZE


/// @brief Data structures to help writing to files need specific packing
#pragma pack(push, 1) // File writing helpers

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describes 3A & Tuning metadata header
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct DebugDataStartHeader
{
    UINT32  dataSize;               ///< Size in bytes of data including the string marker preceeding this struct
    // Debug data version information
    UINT8   majorRevision;          ///< Major revision number
    UINT8   minorRevision;          ///< Minor revision number
    UINT16  patchRevision;          ///< Patch revision number
    // SW version information
    UINT16  SWMajorRevision;        ///< SW Major revision
    UINT16  SWMinorRevision;        ///< SW Minor revision
    UINT32  SWPatchRevision;        ///< SW Patch revision
    CHAR    featureDesignator[2];   ///< SW Specific feature added designator
};
#pragma pack(pop) // File writing helpers: 1


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHISTATSPROPERTYDEFINES_H
