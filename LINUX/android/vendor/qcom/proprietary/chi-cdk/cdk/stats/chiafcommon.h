////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chiafcommon.h
/// @brief Auto Focus & Hybrid Auto Focus Common parameters are defined here.
/// Note: Some parameters are defined as pointers. They refer to memory in the property pool and is to avoid  unnecessary
/// copying of the data. The lifetime of these pointers are valid only during the specific request where these parameters
/// are passed. Each algorithm needs to make an internal copy of the data if it needs to access it later outside this scope.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NOWHINE FILE GR016: Typedefs must not be used for structs, classes, and enumerations

#ifndef CHIAFCOMMON_H
#define CHIAFCOMMON_H

#include "chistatsinterfacedefs.h"
#include <pdlib/chipdlibinterface.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// @brief Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

static const UINT32 MaxAFMotionSensorDirections         = 3;    ///< Maximum vector directions - X/Y/Z
static const UINT8  MaxAFBAFHighPassFilter              = 22;   ///< Maximum size of high pass filter
static const UINT8  MaxAFYConfig                        = 3;    /// Maximum array size of Y Channel configuration
static const UINT8  MaxAFBAFGammaEntries                = 32;   ///< Maximum number of gamma entries
static const UINT8  MaxAFBAFFilterCoringSize            = 17;   ///< Maximum size of Bayer Focus filter coring
static const UINT8  MaxStatsROI                         = 10;   ///< Max Region of Interests(RoI) that can be passed to
                                                                /// algorithm
static const UINT8  MaxBAFFIRFilterSize                 = 13;   ///< Maximum size of Bayer Focus FIR filter
static const UINT32 MaxFWRegions                        = 281;  ///< Maximum number of Floating Windows that can be configured
static const UINT32 MaxSWIIRCoeffA                      = 15;   ///< Maximum number of IIR Filter A coefficients for software stats
static const UINT32 MaxSWIIRCoeffB                      = 15;   ///< Maximum number of IIR Filter A coefficients for software stats
static const UINT32 MaxSWFIRCoeff                       = 15;   ///< Maximum number of Software FIR Filter coefficients
static const UINT16 MaxAFGyroSampleSize                 = 32;   ///< Maximum number of gyro samples per frame
static const UINT16 MaxAFGravitySampleSize              = 1;    ///< Maximum number of gravity samples per frame
static const UINT32 MaxAFZonesSupported                 = 30;   ///< Maximum number of Focus zones supported
static const UINT32 MaxAFKernelsSupported               = 10;   ///< Maximum number of AF kernels supported
static const UINT8  MaxStringLength                     = 32;   ///< Maximum string length for debug mask

/// @brief Describes the near and far search range
typedef struct
{
    INT32   nearRange;  ///< Near range
    INT32   farRange;   ///< Far range
} AFSearchRange;

/// @brief Represents AEC input parameters
typedef struct
{
    FLOAT   luxIndex;           ///< current lux index
    UINT32  exposureIndex;      ///< current exposure index
    FLOAT   currentLuma;        ///< current luma value
    FLOAT   currentGain;        ///< current AEC gain
    INT     currentFPS;         ///< Current fps
    FLOAT   convergePercent;    ///< current percentage of AEC convergence,100% Convergence means AEC is settled
    FLOAT*  pLumaValues;        ///< bayer grid luma values
    UINT32  lumaValuesCount;    ///< number of bayer grid luma values
    FLOAT   brightnessValue;    ///< brightness value
    FLOAT   apertureValue;      ///< Aperture value calculated from aperture or f-number
    FLOAT   sensitivityValue;   ///< Speed or sensitivity value
    UINT64  exposureTime;       ///< Time value calculated from exposure time (in nanoseconds)
    FLOAT   exposureValue;      ///< Exposure value
    UINT32  pixelsPerRegion;    ///< Pixel per region
    BOOL    isLEDAssisted;      ///< LedAssisted
} AFAECInfo;

/// @brief Enum to indicate what type of ROI information
typedef enum
{
    AFROITypeInvalid    = -1,           ///< Invalid type
    AFROITypeGeneral,                   ///< Default Type
    AFROITypeFace,                      ///< Face ROI Type
    AFROITypeTouch,                     ///< Touch ROI Type
    AFROITypeMax        = 0x7FFFFFFF    ///< Max Type
} AFROIType;

/// @brief ROI dimension
typedef struct
{
    UINT32  left;   ///< Horizontal offset of the region
    UINT32  top;    ///< Vertical offset of the region
    UINT32  width;  ///< Width of the ROI
    UINT32  height; ///< Height of the ROI
    UINT32  weight;  ///< weight of ROI
} AFROIDimension;

/// @brief Detected Face ROI dimensions
typedef struct
{
    UINT32              count;                      ///< Number of ROIs
    UINT32              id[MaxFaceROIs];            ///< Face Id
    StatsWeightedROI    weightedROI[MaxFaceROIs];   ///< ROI dimensions
} AFFaceROIInfo;

/// @brief Weighted ROI
typedef struct
{
    AFROIType           ROIType;                 ///< ROI type
    UINT32              requestId;               ///< Frame Id
    UINT32              numberOfROI;             ///< Number of ROIs
    StatsWeightedROI*   pWeightedROI;            ///< ROI dimension
} AFROIInfo;

/// @brief Gyro data from motion sensor module.
typedef struct
{
    BOOL    enabled;                                   ///< Indicates whether Gyro data is enabled
    UINT16  sampleCount;                               ///< Number of Samples
    FLOAT   pAngularVelocityX[MaxAFGyroSampleSize];    ///< angular velocity along x-axis
    FLOAT   pAngularVelocityY[MaxAFGyroSampleSize];    ///< angular velocity along y-axis
    FLOAT   pAngularVelocityZ[MaxAFGyroSampleSize];    ///< angular velocity along z-axis
    FLOAT   gyroSqr;                                   ///< Calculated gyro square value from gyro sample
    UINT64  timeStamp[MaxAFGyroSampleSize];            ///< timestamp for each sample
} AFGyroData;

/// @brief Linear acceleration input from motion sensor module.
typedef struct
{
    BOOL    enabled;                ///< Indicates whether Accelerometer is enabled
    UINT16  sampleCount;            ///< Number of Samples
    FLOAT*  pLinearAccelerationX;   ///< linear acceleration along X axis
    FLOAT*  pLinearAccelerationY;   ///< linear acceleration along Y axis
    FLOAT*  pLinearAccelerationZ;   ///< linear acceleration along Z axis
} AFLinearAccelerationData;

/// @brief Gravity estimate vector from motion sensor module.
typedef struct
{
    BOOL    enabled;                               ///< Indicates whether gravity vector is enabled
    UINT16  sampleCount;                           ///< Number of Samples
    FLOAT   gravityX[MaxAFGravitySampleSize];      ///< gravity data along X Axis
    FLOAT   gravityY[MaxAFGravitySampleSize];      ///< gravity data along Y Axis
    FLOAT   gravityZ[MaxAFGravitySampleSize];      ///< gravity data along Z Axis
    UINT64  timeStamp[MaxAFGravitySampleSize];     ///< timestamp for sample
} AFGravityData;

/// @brief Information from sensor module
typedef struct
{
    FLOAT   focalLength;            ///< Focal length of the sensor
    FLOAT   fNumber;                ///< f-number or aperture of the lens
    FLOAT   pixelSize;              ///< Pixel size of the sensor
    FLOAT   totalFDistance;         ///< Total f-distance
    UINT32  resolutionHeight;       ///< sensor resolution height
    UINT32  resolutionWidth;        ///< sensor resolution width
    FLOAT   actuatorSensitivity;    ///< Sensitivity of the actuator
    UINT    cropLastLine;           ///< last crop of sensor or last line of camif window
    UINT    cropFirstLine;          ///< top crop of sensor or first line of camif window
    UINT    cropFirstPixel;         ///< first pixel of sensor stream
    UINT    cropLastPixel;          ///< last pixel of sensor stream
    BOOL    isPDAFEnabled;          ///< flag to Indicate if PDAF Is enabled
    FLOAT   minFocusDistance;       ///< Minimum focus distance in meters.
    FLOAT   maxFocusDistance;       ///< Maximum focus distance in meters.
} AFSensorInfo;

/// @brief This structure lists some hardware constraints for floating window configuration.
/// This is not complete set of hardware constraints. For full HW configuration
/// please refer to hardware specs.
typedef struct
{
    UINT16  maxWindowCount;         ///< maximum total number of floating windows supported.
    UINT16  maxWindowWidth;         ///< maximum width of each window
    UINT16  minWindowWidth;         ///< minimum width of each window
    UINT16  maxWindowHeight;        ///< maximum height of each window
    UINT16  minWindowHeight;        ///< minimum height of each window.
    UINT16  minHorizontalOffset;    ///< minimum horizontal offset allowed for any window
    UINT16  maxHorizontalOffset;    ///< maximum horizontal offset allowed for any window
    UINT16  minVerticalOffset;      ///< minimum vertical offset for any window
    UINT16  maxVerticalOffset;      ///< maximum vertical offset for any window
} AFHardwareCapabilityFloatingWindow;

/// @brief Details of type and coefficient size for each kernel.
typedef struct
{
    UINT32  mask;                   ///< mask for filter used - FIR/IIR
    UINT32  FIRKernelCount;         ///< fir a size of kernel coefficients
    UINT32  IIRKernelACount;        ///< IIR a size of kernel coefficients
    UINT32  IIRKernelBCount;        ///< IIR b size of kernel coefficients
    BOOL    isDownScaleSupported;   ///< downscale supported
    BOOL    isCoringVariable;       ///< Is coring fixed / variable
} AFHardwareCapabiltyKernelSpec;

///< @ brief Hardware Capabilities
typedef struct
{
    AFHardwareCapabilityFloatingWindow* pWindowInfo;    ///< Pointer to the floating window ROI configuration
    UINT32                              kernelCount;    ///< number of AF kernels supported.
    AFHardwareCapabiltyKernelSpec*      pKernelInfo;    ///< Pointer to the kernel Info
} AFHardwareCapability;

///< @ brief focus distance information
typedef struct
{
  FLOAT lensFocusDistance; ///< Focus distance in diopter units
} AFFocusDistanceInfo;

/// @brief Time stamp information
typedef struct
{
    UINT32            requestId;    ///< Request Id
    StatsTimeStamp    timeStamp;    ///< Start of exposure timestamp
} AFStatsTimeStamp;

/// @brief Bayer Focus statistics region type
typedef enum
{
    AFBAFRegionInvalid      = -1,           ///< Type Invalid
    AFBAFRegionPrimary,                     ///< Primary region type
    AFBAFRegionSecondary,                   ///< Secondary region type
    AFBAFRegionMax          = 0x7FFFFFFF,   ///< Max Type
} AFBAFRegionType;

/// @brief Bayer Focus statistics ROI information
typedef struct
{
    AFBAFRegionType regionType; ///< Bayer focus stats regions type
    AFROIDimension  roi;        ///< Region of Interest Dimensions
    BOOL            isValid;    ///< Is data valid
} AFBAFStatsROI;

/// @brief BAF Statistics from ISP
typedef struct
{
    UINT32            numberOfBAFROI;                         ///< Number of BAF ROIs
    UINT64*           pH1Sum;                                 ///< Sum of pixels in H1 filter
    UINT64*           pH2Sum;                                 ///< Sum of pixels in H2 filter
    UINT64*           pVSum;                                  ///< sum of pixels in V filter
    UINT64*           pH1Sharpness;                           ///< sharpness of pixels in H1 filter
    UINT64*           pH2Sharpness;                           ///< sharpness of pixels in H2 filter
    UINT64*           pVSharpness;                            ///< sharpness of pixels in v filter
    UINT32*           pH1Number;                              ///< number of pixels in H1 filter
    UINT32*           pH2Number;                              ///< number of pixels in H1 filter
    UINT32*           pVNumber;                               ///< number of pixels in V filter
    StatisticsStartUp startupMode;                            ///< statistics start up mode.
} AFBAFStatistics;

/// @brief PDAF defocus information
typedef struct
{
    UINT32  xWindowCount;       ///< Window Number in X axis
    UINT32  yWindowCount;       ///< Window Number in Y axis
    INT32*  pDefocus;           ///< Defocus Value
    UINT32* pConfidenceLevel;   ///< Confidence level
    FLOAT*  pPhaseDifference;   ///< Raw Phase difference
} AFPDAFDefocusInfo;

/// @brief This is the output from PDAF library
typedef struct
{
    PDTriggerType       triggerType;        ///< PD stat trigger type
    AFPDAFDefocusInfo   pDefocus;           ///< Center window PD Values
    AFPDAFDefocusInfo   pPeripheralDefocus; ///< Peripheral window PD values
    INT                 lens_pos;           ///< Current lens pos
} AFPDAFData;

/// @brief Different confidence levels of Time Of Flight Sensor (TOF) input
typedef enum
{
    AFTOFConfidenceInvalid  = -1,           ///< Type Invalid
    AFTOFNotConfident,                      ///< TOF is not confident on data provided
    AFTOFConfident,                         ///< TOF is cofident distance measured is good
    AFTOFSemiConfident,                     ///< TOF is semi-confident on data measured
    AFTOFConfidenceMax      = 0x7FFFFFFF    ///< Type Max
} AFTOFConfidence;

/// @brief TOF input data to be passed to AF algorithm
typedef struct
{
    UINT32  requestId;              ///< Current request id, can be optional since it relies on timestamp
    INT64   timestamp;              ///< Timestamp of arrival of the laser data
    INT32   distanceInMilliMeters;  ///< Calculated object distance in mm
    INT32   confidence;             ///< Confidence of distance measured
    INT32   nearLimit;              ///< Near end of distance measured in mm (when TOF is semi confident)
    INT32   farLimit;               ///< Far end of distance measured in mm (when TOF is semi confident)
    INT32   maxDistance;            ///< Maximum distance that can be measured
    BOOL    isValid;                ///< Is data valid
} AFTOFData;

/// @brief Computed focus value data to be passed to Contrast Auto Focus Algorithm
typedef struct
{
    UINT64  requestId;                              ///< Current request id
    FLOAT   focusValueHorizontalVertical;           ///< Computed Horizontal and Vertical Focus Value Sum
    FLOAT   focusValueHorizontalVerticalNormalized; ///< Computed Focus Value Normalized Sum
                                                    ///  of Horizontal and Vertical Statistic
    FLOAT   focusValueHorizontal;                   ///< Computed Horizontal Only Focus Value
    FLOAT   focusValueHorizontalNormalized;         ///< Computed Normalized Horizontal Only Focus Value
    FLOAT   focusValueVertical;                     ///< Computed Vertical Only Focus Value
    FLOAT   focusValueVerticalNormalized;           ///< Computed Normalized Vertical Only Focus Value
    BOOL    isStatsValid;                           ///< Indicate whether stats is valid
} AFFocusStatsInfo;

/// @brief Direction to move
typedef enum
{
    AFMoveDirectionInvalid  = -1,           ///< Invalid move direction
    AFMoveDirectionNear,                    ///< Move towards MACRO position
    AFMoveDirectionFar,                     ///< Move towards infinity position
    AFMoveDirectionMax      = 0x7FFFFFFF    ///< Type Max
} AFMoveDirectionType;

/// @brief list of focus mode types supported
typedef enum
{
    AFFocusModeInvalid      = -1,           ///< Focus Mode Invalid
    AFFocusModeAuto,                        ///< Focus Mode Auto
    AFFocusModeContinuousPicture,           ///< Focus Mode Continuous Picture
    AFFocusModeContinuousVideo,             ///< Focus Mode Continuous Video
    AFFocusModeInifinity,                   ///< Focus Mode Infinity
    AFFocusModeMacro,                       ///< Focus Mode Macro
    AFFocusModeManual,                      ///< Focus Mode Manual
    AFFocusModeMax          = 0x7FFFFFFF    ///< Focus Mode type max
} AFFocusMode;

/// @brief List of AF metering types supported.
typedef enum
{
    AFMeteringInvalid           = -1,           ///< Invalid type
    AFMeteringAuto,                             ///< Auto Metering
    AFMeteringSpot,                             ///< Spot Metering
    AFMeteringCenterWeighted,                   ///< Center Weighted Metering
    AFMeteringAverage,                          ///< Average Metering
    AFMeteringTypeMax           = 0x7FFFFFFF    ///< Type max
} AFMeteringType;

/// @brief commands to control auto-focus operations in AUTO mode
typedef enum
{
    AFInvalidCommand        = -1,           ///< Invalid Command
    AFStartCommand,                         ///< Start Auto Focus Command
    AFStopCommand,                          ///< Stop Auto Focus Command
    AFUnLockCommand,                        ///< Auto Focus UnLock Command
    AFLockCommand,                          ///< Auto Focus Lock Command
    AFControlCommandTypeMax = 0x7FFFFFFF    ///< Type max
} AFControlCommand;

/// @brief camera run mode type video/snapshot
typedef enum
{
    AFRunModeInvalid    = -1,           ///< Invalid Run Mode
    AFRunModePreview,                   ///< Camera preview run mode
    AFRunModeSnapshot,                  ///< Snapshot Run Mode
    AFRunModeVideo,                     ///< Video Run Mode
    AFRunModeMax        = 0x7FFFFFFF    ///< Type max
} AFRunMode;

///@ brief Data structure definition for Preview Assisted AF (PAAF).
///  This is software stats generated after processing preview request.
typedef struct
{
    UINT64  requestId;  ///< Frame-id of the preview request from which preview stats are calculated
    UINT32  focusValue; ///< Focus value
} AFSoftwareStatistics;

/// @brief Enumeration describing Auto Focus status
typedef enum
{
    AFAlgoStatusTypeInvalid        = -1,        ///< Auto focus is in error state
    AFAlgoStatusTypeInitialized,                ///< Auto focus is in initialized state
    AFAlgoStatusTypeFocusing,                   ///< Auto focus is currently focusing
    AFAlgoStatusTypeFocused,                    ///< Auto focus is in focused state
    AFAlgoStatusTypeNotFocused,                 ///< Auto focus is not focused state
    AFAlgoStatusTypeMax            = 0x7FFFFFFF ///< Type max
} AFAlgoStatusType;

/// @brief Structure describing AF status
typedef struct
{
    BOOL                focusDone;              ///< Focus is done flag
    AFAlgoStatusType    status;                 ///< Current focusing status
    FLOAT               focusDistanceNear;      ///< Near distance where an object appears to be in focus(in milli-meters)
    FLOAT               focusDistanceOptimal;   ///< Optimal distance where an object appears to be in focus(in milli-meters)
    FLOAT               focusDistanceFar;       ///< Far distance where an object appears to be in focus(in milli-meters)
} AFAlgoStatusParam;

/// @brief Structure describing top level BAF stats configuration
typedef struct
{
    UINT32          horizontalNum;          ///< Number of horizontal regions
    UINT32          verticalNum;            ///< Number of vertical regions
    AFROIDimension  ROI;                    ///< BAF ROI configuration
    INT             scale;                  ///< BAF scale
    UINT            minChannelGainR;        ///< Minimum Red channel value
    UINT            minChannelGainGr;       ///< Minimum GreenRed channel value
    UINT            minChannelGainGb;       ///< Minimum GreenBlue channel value
    UINT            minChannelGainB;        ///< Minimum Red channel value
    UINT32          numberOfHighPassFilter; ///< Number of high pass filter configuration
    INT*            pHighPassFilter;        ///< High pass filter configuration
} AFBAFConfig;

/// @brief Enumeration describing BAF stats channel gains.
typedef enum
{
    BAFChannelSelectTypeInvalid = -1,           ///< BAF channel invalid
    BAFChannelSelectTypeG,                      ///< G channel type
    BAFChannelSelectTypeY,                      ///< Y channel type
    BAFChannelSelectTypeMax     = 0x7FFFFFFF    ///< Max Type
} AFBAFChannelSelectType;

/// @brief Enumeration describing input channel type.
typedef enum
{
    BAFInputSelectGInvalid  = -1,           ///< Invalid type
    BAFInputSelectGr,                       ///< Select GR channel for BAF
    BAFInputSelectGb,                       ///< Select GB channel for BAF
    BAFInputSelectMax       = 0x7FFFFFFF    ///< Max Type

} AFBAFInputGSelectType;

/// @brief Structure describing input config parameters.
typedef struct
{
    BOOL                    isValid;            ///< Is the configuration valid
    AFBAFChannelSelectType  BAFChannelSel;      ///< Channel selection config
    AFBAFInputGSelectType   BAFInputGSel;       ///< G channel selection config
    UINT32                  numberOfYConfig;    ///< Number of Y config
    FLOAT*                  pYConfig;           ///< Y channel config
} AFBAFInputConfig;

/// @brief Structure describing look up table config
typedef struct
{
    UINT32    numGammaLUT;  ///< Number of entries in the Gamma LUT
    UINT32*   pGammaLUT;    ///< Gamma Look up table
} AFBAFGammaLUTConfig;

/// @brief Structure describing BAF scale configuration.
typedef struct
{
    BOOL    BAFScaleEnable; ///< Flag to enable BAF scalar
    INT32   scaleM;         ///< M scalar value
    INT32   scaleN;         ///< N scalar value
} AFBAFScaleConfig;

/// @brief Structure describing BAF filter type.
typedef enum
{
    AFBAFFilterTypeInvalid        = -1,         ///< Invalid type
    AFBAFFilterTypeHorizontal1,                 ///< Index for Horizontal 1 BAF filter
    AFBAFFilterTypeHorizontal2,                 ///< Index for Horizontal 2 BAF filter
    AFBAFFilterTypeVertical,                    ///< Index for Vertical BAF filter
    AFBAFFilterTypeMax            = 0x7FFFFFF   ///< Max Type
} AFBAFFilterType;


/// @brief Structure describing FIR filter configuration
typedef struct
{
    BOOL      enable;                   ///< FIR filter config enable
    UINT32    numOfFIRCoefficients;     ///< Number of filter coefficient
    INT32*    pFIRFilterCoefficients;   ///< FIR filter coefficient
} AFBAFFIRFilterConfig;

/// @brief Structure describing IIR filter configuration.
/// 1st biquad: [b10, b11, b12, -a11, -a12]
/// 2nd biquad: [b20, b21, b22, -a21, -a22]
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
} AFBAFIIRFilterConfig;

/// @brief Structure describing Core filter configuration
typedef struct
{
    INT32     threshold;            ///< BAF filter threshold
    UINT32    gain;                 ///< BAF filter gain
    UINT32    numberOfCoringFilter; ///< Number of core filters
    UINT32*   pCoringFilter;        ///< BAF filter cores
} AFBAFFilterCoringConfig;

/// @brief Structure describing BAF filter configuration
typedef struct
{
    BOOL                        isValid;                ///< Is Filter config valid
    BOOL                        horizontalScaleEnable;  ///< Is horizontal scale enabled
    AFBAFFIRFilterConfig*       pBAFFIRFilterConfig;    ///< Pointer to FIR filter configuration
    UINT32                      sizeOfFIRFilterConfig;  ///< size of FIR filter config
    AFBAFIIRFilterConfig*       pBAFIIRFilterConfig;    ///< Pointer to IIR filter configuration
    UINT32                      sizeOfIIRFilterConfig;  ///< size of IIR filter config
    INT32                       shiftBits;              ///< Number of bits to shift
    AFBAFFilterCoringConfig*    pBAFFilterCoringConfig; ///< Pointer to BAF coring configuration
    UINT32                      sizeOfCoringConfig;    ///< size of coring config
} AFBAFFilterConfig;


/// @brief Structure describing bayer focus firmware stats configuration parameters
typedef struct
{
    AFBAFInputConfig*       pBAFInputConfig;        ///< BAF input configuration
    UINT32                  sizeOfInputConfig;      ///< Size of input config
    AFBAFGammaLUTConfig*    pBAFGammaLUTConfig;     ///< BAF Gamma LUT configuration
    UINT32                  sizeOfGammaLUTConfig;   ///< Size of gamma LUT config
    AFBAFScaleConfig*       pBAFScaleConfig;        ///< BAF Scale configuration
    UINT32                  sizeOfScaleConfig;      ///< Size of scale config
    AFBAFFilterConfig*      pBAFFilterConfigH1;     ///< BAF filter configuration
    UINT32                  sizeOfFilterConfigH1;   ///< Size of filter config
    AFBAFFilterConfig*      pBAFFilterConfigH2;     ///< BAF filter configuration
    UINT32                  sizeOfFilterConfigH2;   ///< Size of filter config
    AFBAFFilterConfig*      pBAFFilterConfigV;      ///< BAF filter configuration
    UINT32                  sizeOfFilterConfigV;    ///< Size of filter config
} AFBAFFloatingWindowConfig;

/// @brief Structure describing region of interest type
typedef enum
{
    AFBAFFloatingWindowROITypeInvalid    = -1,          ///< Type Invalid
    AFBAFFloatingWindowROITypeDefault,                  ///< BAF stats default ROI type
    AFBAFFloatingWindowROITypeCustom,                   ///< BAF stats custom ROI type
    AFBAFFloatingWindowROITypeMax        = 0x7FFFFFFF   ///< Max Type
} AFBAFFloatingWindowROIType;


/// @brief Enumeration describing configuration window type
typedef enum
{
    AFBAFFloatingWindowTypeInvalid       = -1,          ///< Type Invalid
    AFBAFFloatingWindowTypePrimary,                     ///< BAF stats primary region type
    AFBAFFloatingWindowTypeSecondary,                   ///< BAF stats secondary region type
    AFBAFFloatingWindowTypeMax           = 0x7FFFFFFF   ///< Max Type
} AFBAFFloatingWindowType;

/// @brief Structure describing BAF ROI dimension
typedef struct
{
    AFBAFFloatingWindowType    regionType;  ///< BAF region type
    AFROIDimension             ROI;         ///< ROI coordinates to configure
    UINT32                     regionNum;   ///< Region Number
    BOOL                       isValid;    ///< Is ROI valid
} AFBAFFloatingWindowROIParam;


/// @brief Structure describing BAF ROI configuration
typedef struct
{
    BOOL                            isValid;            ///< Is config type valid
    AFBAFFloatingWindowROIType      BAFROIType;         ///< ROI type
    UINT32                          numberOfROI;        ///< Number of ROI regions
    AFBAFFloatingWindowROIParam*    pBAFFwROIDimension; ///< BAF ROI configuration
} AFBAFFloatingWindowROIConfig;

/// @brief Structure describing the software stats filter config
typedef struct
{
    BOOL            enable;                     ///< enable/disable software stats
    AFROIDimension  ROI;                        ///< region of interest
    UINT32          numberOfCoefficientsA;      ///< Number of A coefficients
    FLOAT*          pCoefficientsIIRA;          ///< A coefficient values
    FLOAT           numberOfCoefficientsB;      ///< Number of B Coefficients
    FLOAT*          pCoefficientsIIRB;          ///< B coefficient values
    FLOAT           numberOfCoefficientsFIR;    ///< Number of FIR Coefficients
    FLOAT*          pCoefficientsFIR;           ///< FIR Coefficients values
    double          minimumFocusValue;          ///< Minimum Focus Value
} AFSoftwareStatisticsFilterConfig;


/// @brief Structure describing top level PDAF ROI configuration
typedef struct
{
    UINT32          horizontalNum;  ///< Number of horizontal regions
    UINT32          verticalNum;    ///< Number of vertical regions
    AFROIDimension  AFFixedWindow;  ///< Window configuration
} AFAlgoPDAFWindowConfig;

/// @brief Represents an AF input parameter
typedef struct
{
    BOOL    afExif;                 ///< Settings to enable/disable debug data in exif
    BOOL    disableFocusIndication; ///< Settings to override static indicator status
    BOOL    fovcEnable;             ///< Settings to enable/disable Field of View compensation feature
    BOOL    mwEnable;               ///< Settings to enable/disable Multiwindow feature
    UINT    enableAFAlgo;           ///< Settings mask to enable different auto focus algorithms
    UINT    afFullsweep;            ///< Settings to enable disable fullsweep mode
    UINT    manualAf;               ///< Settings to enable/disable manual auto focus
    UINT    lensPos;                ///< Settings to provide the lens position for manual af control
    BOOL    kpiDebug;               ///< Settings to enable/disable KPI Systrace
    BOOL    isSpdEnable;           ///< Settings to enable/disable Spotlight Detection
    UINT    spdStatsType;           ///< Settings to set the spotlight detection stats type
    BOOL    enableHJAF;             ///< Settings to enable/disable Hand Jitter af
    CHAR    vmDebugMask[MaxStringLength]; ///< Settings to control VM logs based on VM group
    BOOL    disablePDAF;            ///< Settings to enable/disable PDAF
} AFAlgoSettingInfo;

/// @brief Enumeration for lens movement direction.
typedef enum
{
    AFMoveLensInvalid = -1,         ///< Type Invalid
    AFMoveLensNear,                 ///< Move the lens towards near object
    AFMoveLensFar,                  ///< Move the lens towards far object
    AFMoveLensMax     = 0x7FFFFFFF  ///< Type Max
} AFMoveLensDirectionType;

/// @brief Details of AF move lens information.
typedef struct
{
    UINT32   lensPositionInLogicalUnits;    ///< Logical lens position to move the lens to
} AFLogicalLensMoveInfo;

/// @brief  Details of AF move lens information. DAC positions
typedef struct
{
    UINT32  lensPositionInDACUnits;  ///< Dac position to move the lens to
} AFDACLensMoveInfo;

/// @brief Details of lens movement type
typedef enum
{
    AFLensMoveTypeInvalid = -1,         ///< Type Invalid
    AFLensMoveTypeLogical,              ///< Move lens using steps
    AFLensMoveTypeDAC,                  ///< Move lens using dac
    AFLensMoveTypeMax     = 0x7FFFFFFF  ///< Type Max
} AFLensMoveType;

/// @brief Represents Move Lens type
typedef struct
{
    AFLensMoveType         moveType;            ///< Information required to move the lens
    AFLogicalLensMoveInfo  logicalLensMoveInfo; ///< Details of AF move lens using logical value
    AFDACLensMoveInfo      digitalLensMoveInfo; ///< Details of AF move lens using DAC value
} AFLensPositionInfo;

/// @brief Represents FOV compensation factor
typedef struct
{
    FLOAT         fieldOfViewCompensationFactor;            ///< Information required to apply crop factor
} AFFOVCInfo;

/// @brief  Exposure Compensation Enable/Disable Flag
typedef struct
{
    BOOL    exposureCompensationEnable; ///< Exposure compensation enable/disable flag
} AFAlgoExposureCompensation;

/// @brief  Focus Value
typedef struct
{
    FLOAT   focusValue; ///< Calculated focus value
} AFAlgoFocusValue;

/// @brief  Spot Light Detected
typedef struct
{
    BOOL    spotLightDetected;  ///< Spotlight detection status
} AFAlgoSpotLightInfo;

/// @brief  Crop Magnification Factor
typedef struct
{
    FLOAT   cropMagnificationFactor;    ///< Calculated crop factor based on lens position
} AFAlgoCropMagnificationFactor;

/// @brief Identifies the type of create parameter
typedef enum
{
    AFCreateParamTypeInvalid                    = -1,           ///< Type Invalid
    AFCreateParamTypeTuningParams,                              ///< Tuning Parameters
    AFCreateParamTypeLogFunctionPtr,                            ///< Logging function pointer
    AFCreateParamTypeSettingsPtr,                               ///< Settings Data Pointer
    AFCreateParamTypeCreateCustomAFFunctionPtr,                 ///< Function pointer to create custom AF algorithm
    AFCreateParamTypeCreateCustomHAFFunctionPtr,                ///< Function pointer to create custom HAF algorithms
    AFCreateParamTypeSessionHandle,                             ///< Stats Session Handle
    AFCreateParamTypeCameraOpenIndicator,                       ///< Actual Camera Open state
    AFCreateParamTypeCameraInfo,                                ///< Set camera information regarding algo role(master/slave)
                                                                ///  and camera ID during algorithm creation.
                                                                ///  Payload type: StatsCameraInfo
    AFCreateParamTypeCount,                                     ///< Create Param Type Count
    AFCreateParamTypeMax                        = 0x7FFFFFFF    ///< Max Create Param Type
} AFAlgoCreateParamType;

/// @brief Represents an AF input parameter
typedef struct
{
    AFAlgoCreateParamType  createParamType; ///< Type of parameter passed
    VOID*                  pParam;          ///< Payload of the particular parameter type
    UINT32                 sizeOfParam;     ///< Size of the payload.
} AFAlgoCreateParam;

/// @brief Represents AF algorithm creation parameters
typedef struct
{
    AFAlgoCreateParam* pParamList;    ///< Pointer to AF create-parameter
    UINT32             paramCount;    ///< Number of input create-parameters
} AFAlgoCreateParamList;

/// @brief Identifies the type of destroy parameter
typedef enum
{
    AFDestroyParamTypeInvalid               = -1,           ///< Type Invalid
    AFDestroyParamTypeCameraCloseIndicator,                 ///< Camera Close Indicator
                                                            ///< UINT 0 - Camera Close 1 Camera Open
    AFDestroyParamTypeCameraInfo,                           ///< Camera Info
                                                            ///  Payload: StatsCameraInfo
    AFDestroyParamTypeCount,                                ///< Destroy Param Type Count
    AFDestroyParamTypeMax                   = 0x7FFFFFFF    ///< Max Destroy Param Type
} AFAlgoDestroyParamType;

/// @brief Represents an AF destroy parameter
typedef struct
{
    AFAlgoDestroyParamType  destroyParamType;       ///< Type of parameter passed
    VOID*                   pParam;                 ///< Payload of the particular parameter type
    UINT32                  sizeOfParam;            ///< Size of the payload.
} AFAlgoDestroyParam;

/// @brief Represents AF algorithm destroy parameters
typedef struct
{
    AFAlgoDestroyParam* pParamList;    ///< Pointer to AF create-parameter
    UINT32              paramCount;    ///< Number of input destroy-parameters
} AFAlgoDestroyParamList;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHIAFCOMMON_H
