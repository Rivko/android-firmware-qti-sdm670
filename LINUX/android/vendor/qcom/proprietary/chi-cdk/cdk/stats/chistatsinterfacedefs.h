////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chistatsinterfacedefs.h
/// @brief Statistics interfaces types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NOWHINE FILE GR016: Typedefs must not be used for structs, classes, and enumerations

#ifndef CHISTATSINTERFACEDEFS_H
#define CHISTATSINTERFACEDEFS_H

#include "camxcdktypes.h"
#include "chistatspropertydefines.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// @brief Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

// Statistics common static definitions
static const UINT32 StatisticsBayerChannelsCount        = 4;    ///< Total number of statistics Bayer channels
static const UINT32 StatisticsYStatsRows                = 16;   ///< Y stats number of rows
static const UINT32 StatisticsYStatsCols                = 16;   ///< Y stats number of columns
static const UINT32 StatisticsYStatsNum                 =       ///< Size of the Y Sum array for AEC output
                                                          StatisticsYStatsRows * StatisticsYStatsCols;
static const UINT32 StatisticsMaxNumOfLED               = 2;    ///< The maximum number of LEDs on device
static const UINT32 StatisticsMaxROI                    = 16;   ///< Maximum number of ROI, that could given to stats module
static const UINT32 StatisticsFaceHistogramBinSize      = 256;  ///< Histogram Bin size
static const UINT32 StatisticsMaxLEDCalibrationPoints   = 12;   ///< Max stats calibration points for dual LED

static const UINT32 StatisticsMaxRowSumHRegions         = 16;   ///< Max number of row sum horizontal regions
static const UINT32 StatisticsMaxRowSumStats            = 4096; ///< Max number of row sum stats

static const UINT32 StatisticsBHistMaxBinSize           = 1024; ///< Bayer histogram max bin size
static const UINT32 StatisticsBGStatsNum                = 3072; ///< Bayer grid stats number of regions

// Sensor capabilities
static const UINT32 StatisticsSensorAFSupport           = 0x00000001;   ///< Set the mask, if auto focus is supported
                                                                        ///  by the sensor
static const UINT32 StatisticsSensorOISSupport          = 0x00000002;   ///< Set the mask, if optical imge stabilization
                                                                        ///  is supported by the sensor

/// @brief Maximum number of vendor tags supported
static const UINT32 StatisticsMaxVendorTag              = 50;   ///< Note that this vealue can be increased
                                                                ///  but it should never be decreased.
                                                                ///  It will break backward compatibility otherwise

static const UINT32 StatsCCMRowCount                    = 3;    ///< number of rows in the color correction matrix (CCM)
static const UINT32 StatsCCMColCount                    = 3;    ///< number of columns in the color correction matrix (CCM)
static const UINT8  MaxCCM = 3;

/// @brief Defines algorithm handle.
typedef VOID* StatsAlgorithmHandle;

/// @brief Stats algorithm Version
typedef struct
{
    UINT32  majorVersion;   ///< Major version of the algorithm
    UINT32  minorVersion;   ///< Minor version of the algorithm
    UINT32  patchVersion;   ///< Patch version of the algorithm
} StatsAlgorithmVersion;

/// @brief Structure describing the rectangle coordinates
#pragma pack(push, 1)
typedef struct
{
    UINT32   left;      ///< The x coordinate of the StatsRectangle
    UINT32   top;       ///< The y coordinate of the StatsRectangle
    UINT32   width;     ///< The width of the StatsRectangle
    UINT32   height;    ///< The height of the StatsRectangle
} StatsRectangle;
#pragma pack(pop)
/// @brief Container for stats any data of arbitrary length
typedef struct
{
    VOID*   pData;      ///< Pointer to the data
    UINT32  dataSize;   ///< Size in bytes of the data
} StatsDataPointer;

/// @brief Container for stats any data of arbitrary length
typedef struct
{
    VOID*   pTuningSetManager;      ///< Pointer to TuningDataManager object
    VOID*   pTuningModeSelectors;   ///< Pointer to tuning mode selector.
    UINT    numSelectors;           ///< Number of elements in the pSelectors array. Must be greater than 0
}StatsTuningData;

/// @brief: Structure representing generic two dimensional value
typedef struct
{
    UINT32  width;  ///< Width of the rectangle
    UINT32  height; ///< Height of the rectangle
} StatsDimension;

/// @brief Stats antibanding types
typedef enum
{
    StatisticsAntibandingInvalid = -1,          ///< Antibanding Invalid type
    StatisticsAntibandingOff,                   ///< Antibanding off
    StatisticsAntibanding50Hz,                  ///< Antibanding mode set to 50Hz
    StatisticsAntibanding60Hz,                  ///< Antibanding mode set to 60Hz
    StatisticsAntibandingAuto,                  ///< Antibanding automatic mode
    StatisticsAntibandingAuto_50HZ,             ///< Antibanding automatic mode start at 50HZ
    StatisticsAntibandingAuto_60HZ,             ///< Antibanding automatic mode start at 60HZ
    StatisticsAntibandingMax     = 0x7FFFFFFF   ///< Anchor to indicate the last item in the defines
}StatisticsAntibanding;

/// @brief Stats start up validity enum to handle PCR before stream on for AEC
typedef enum
{
    StatisticsStartUpValid = 0,             ///< valid case
    StatisticsStartUpInvalid                ///< invalid case
}StatisticsStartUp;

/// @brief Statistics LED state
typedef enum
{
    StatisticsLEDInvalid    = -1,           ///< LED state Invalid
    StatisticsLEDOff,                       ///< LED state off
    StatisticsLEDLow,                       ///< LED state low
    StatisticsLEDHigh,                      ///< LED state high
    StatisticsLEDInit,                      ///< LED state initialized
    StatisticsLEDRelease,                   ///< LED state released
    StatisticsLEDMax        = 0x7FFFFFFF    ///< Anchor to indicate the last item in the defines
}StatisticsLEDState;

typedef enum
{
    AECAlgoExposureInvalid = -1,                  ///< Indicates invalid or there is no multiple sets of expo stats
    AECAlgoExposureDefault =  0,                  ///< Default exposure when we don't have multi-set, same as short for now
    AECAlgoExposureLEF,                           ///< Represents the Long exposure information
    AECAlgoExposureNum,                           ///< Represents the count of different exposure set
} AECAlgoMultiExposureType;


/// @brief Statistics LED state
typedef struct
{
    FLOAT flashOff;                     ///< Sensitivity value when the flash is off
    FLOAT flashLow;                     ///< Sensitivity value when the flash is low
    FLOAT flashHigh;                    ///< Sensitivity value when the flash is high
} StatsLEDFlashSensitivity;

/// @brief Face Histogram information
typedef struct
{
    UINT32 sampleCount;                         ///< Number of samples
    UINT32 bin[StatisticsFaceHistogramBinSize]; ///< Histogram bin
} StatsFaceHistogram;

/// @brief Base structure to define single face information
typedef struct
{
    StatsRectangle   roi;           ///< Face boundary with respect to active pixel window.
                                    ///  See activePixelWindow in StatsWindowInfo for details
    INT32            confidence;    ///< Confidence for determining the prominence. The value
                                    ///  range from 0 to 100
    INT32            faceID;        ///< Detected face unique ID
} StatsFaceData;

/// @brief The structure provides face information from face detection algorithm to statistics algorithms
typedef struct
{
    UINT32              faceCount;                              ///< Number of faces detected
    StatsFaceData       face[StatisticsMaxROI];                 ///< Data for each face detected
    StatsFaceData       faceOriginPosition[StatisticsMaxROI];   ///< Origin face position before translate
    StatsFaceHistogram* pHistogram[StatisticsMaxROI];           ///< Pointer the the histogram of detected faces.
                                                                ///  Deep copy of data required if consumed.
    UINT32              requestID;                              ///< Sequence of the frame were the faces were detected
    BOOL                regionPriority;                         ///< Indicate if region priority is enable for 3A face info.
} StatsFaceInformation;

/// @brief Defines AWB decision/illuminant source types
typedef enum
{
    StatsIlluminantInvalid        = -1,           ///< Pre-flash state Invalid
    StatsIlluminantHigh,                          ///< High CCT range
    StatsIlluminantD75,                           ///< Standard D75 light
    StatsIlluminantD65,                           ///< Standard D65 light
    StatsIlluminantD50,                           ///< Standard D50 light
    StatsIlluminantCW,                            ///< Standard Continuous Wave (CW) light
    StatsIlluminantFluorescent,                   ///< Flourescent light
    StatsIlluminantTL84,                          ///< Standard TL84 light
    StatsIlluminantIncandescent,                  ///< Incandescent light
    StatsIlluminantHorizon,                       ///< Horizon type of light
    StatsIlluminantLow,                           ///< Low CCT range
    StatsIlluminantCount,                         ///< Stats Illuminant type count
    StatsIlluminantMax            = 0x7FFFFFFF    ///< Anchor to indicate the last item in the defines
} StatsIlluminantType;

/// @brief Structure used to provide AEC and/or AWB with the results of LED calibration
typedef struct
{
    UINT8   calibratedPointCount;                       ///< Number of combinations calibrated
    FLOAT   ratioRG[StatisticsMaxLEDCalibrationPoints]; ///< RG ratio from calibration
    FLOAT   ratioBG[StatisticsMaxLEDCalibrationPoints]; ///< BG ratio from calibration
} StatsLEDCalibrationDataInput;

/// @brief: Structure representing Sensor AEC Capping Info
typedef struct
{
    INT64 minSEFExposureTime;
    INT64 maxSEFExposureTime;
    INT64 minMEFExposureTime;
    INT64 maxMEFExposureTime;
    INT64 minLEFExposureTime;
    INT64 maxLEFExposureTime;
} StatsAECCapInfo;

/// @brief: Structure representing Sensor AEC Capping Info
typedef struct
{
    UINT32 fusion_luma_thres1_exp0;
    UINT32 fusion_luma_thres2_exp0;
    FLOAT  tm_factor;
    FLOAT  tm_boost_strength;
    FLOAT  tm_pre_gamma;
    FLOAT  tm_maxgain;
    FLOAT  tm_mingain;
    FLOAT  tm_max_total_gain;
} StatsSHDRParams;

/// @brief Container for generic sensor information required by stats module
typedef struct
{
    UINT32  currentFPS;             ///< Current frame rate, multiply fps*Q8
    UINT32  maxFPS;                 ///< Maximum frame rate, multiply fps*Q8
    UINT32  currentLinesPerFrame;   ///< Current lines per frame
    UINT32  currentMaxLineCount;    ///< Current maximum line count
    FLOAT   maxGain;                ///< Maximum gain
    UINT32  pixelClock;             ///< Pixel clock value
    UINT32  pixelClockPerLine;      ///< Pixel clock per line
    UINT64  sensorCapabilities;     ///< Sensor capabilities supported
                                    ///  See Sensor capabilities for details
    UINT32  sensorResWidth;         ///< Sensor resolution width
    UINT32  sensorResHeight;        ///< Sensor resolution height
    UINT16  pixelSumFactor;         ///< Pixel sum factor
    FLOAT   fNumber;                ///< Focal ratio
    UINT32  sensorActiveResWidth;   ///< Sensor active resolution width
    UINT32  sensorActiveResHeight;  ///< Sensor active resolution height
    StatsAECCapInfo sensorAECCapstime; ///< Sensor Exposre time Capping
} StatsSensorInfo;

/// @brief: Structure representing ISP stream information
typedef struct
{
    StatsRectangle  activePixelWindow;  ///< Dimensions of the active pixel window, where the statistics collection is based on.
                                        ///  This window corresponds to any crop that is done for removing inactive
                                        ///  pixels/black pixels
    StatsRectangle  cropWindow;         ///< Dimensions of the crop window with respect to the active pixel window.
                                        ///  See activePixelWindow in StatsWindowInfo for details
} StatsWindowInfo;

/// @brief Structure describing the supported statistic capabilities
typedef struct
{
    StatsRectangle  activePixelWindow;          ///< The default stats window size without any cropping
    UINT8           maxBitDepth;                ///< The maximum bit depth for each value in the region
    UINT8           maxHWStatsSkippingFactor;   ///< Maximum factor for hardware stats skipping, with 0 being no support for
                                                ///  skiping. Value is in log-2 format.
                                                ///  Only used when HWStatsCapabilitySkipping capability is enabled
    UINT32          HWStatsCapabilities;        ///< Bit-mask of HWStatsCapabilityXXX values
} StatsCapability;

///< @brief Enumeration decribing how statistics ROI should be configured
typedef enum
{
    StatsROISelectionCustom     = 0,    ///< Use custom ROI provided
    StatsROISelectionCroppedFOV,        ///< Set the ROI to the cropped FOV
    StatsROISelectionFullFOV,           ///< Set the ROI to the full uncropped window
} StatsROISelection;

/// @brief Structure describing BG and BE stats configuration
typedef struct
{
    UINT32              horizontalRegionCount;                                 ///< Number of horizontal regions
    UINT32              verticalRegionCount;                                   ///< Number of vertical regions
    StatsRectangle      ROI;                                                   ///< ROI coordiates to configure relative
                                                                               ///  to active pixel window.  See
                                                                               ///  activePixelWindow in StatsWindowInfo. Only
                                                                               ///  used when ROISelection is set to
                                                                               ///  StatsROISelectionCustom
    UINT32              channelGainThreshold[StatisticsBayerChannelsCount];    ///< Max threshold for each channel
    UINT32              minChannelLumaThreshold[StatisticsBayerChannelsCount]; ///< Min threshold for each channel. Supported if
                                                                               ///  HWStatsCapabilityMinLuma is present
    StatsROISelection   ROISelection;                                          ///< Indicates how the ROI is configured
    UINT8               outputBitDepth;                                        ///< The output bit depth for each value. This
                                                                               ///  must be smaller than
                                                                               ///      StatsCapability::maxBitDepth
    BOOL                enableYStatsComputation;                               ///< Enables the HW YStats computation
    FLOAT               YStatsWeights[RGBChannelCount];                        ///< YStats channel weights. Indexed by one of
                                                                               ///  ChannelIndexX values
    BOOL                enableSaturationStats;                                 ///< Enables HW Saturation stats output
    BOOL                enableQuadSync;                                         ///< Enables quad sync
    UINT8               HWStatsSkippingFactor;                                 ///< Skip pattern factor in log-2 format, capped
                                                                               ///  by StatsCapability::maxHWStatsSkippingFactor
} StatsBayerGridBayerExposureConfig;

/// @brief Structure describing BHIST stats configuration
typedef struct
{
    ColorChannel        channel;                                               ///< Channel Info
    BOOL                uniform;                                               ///< Flag to indicate uniform Histogram
    StatsRectangle      ROI;                                                   ///< ROI
    StatsROISelection   ROISelection;                                          ///< ROI selecttion
} StatsBayerHistogramConfig;

///< @brief Defins the Row Sum(RS) statistic data structure
typedef struct
{
    UINT32  rowSumCount;                    ///< Number of row sums
    UINT32  horizontalRegionCount;          ///< Number of horizontal regions
    UINT32  verticalRegionCount;            ///< Number of vertical regions
    UINT32  regionHeight;                   ///< row sum region height
    UINT32  regionWidth;                    ///< row sum region width
    UINT32* rowSum;                         ///< Pointer to the row sum 2-D array of size
                                                ///  StatisticsMaxRowSumHRegions by StatisticsMaxRowSumStats
} StatsRowSum;

/// @brief Defines the different type of color channel
typedef enum
{
    StatsColorChannelInvalid    = -1,           ///< Not a valid color type
    StatsColorChannelR,                         ///< Color channel red
    StatsColorChannelGR,                        ///< Color channel green-red
    StatsColorChannelGB,                        ///< Color channel green-blue
    StatsColorChannelB,                         ///< Color channel blue
    StatsColorChannelG,                         ///< Color channel green
    StatsColorChannelY,                         ///< Color channel Y(luma)
    StatsColorChannelMax        = 0x7FFFFFFF    ///< Anchor to indicate the last item in the defines
} StatsColorChannelType;

///< @brief Defines statistic data for each color channel excluding saturation
typedef struct StatsBayerGridChannelInfoS
{
    UINT32      RSum;               ///< Red sum of the pixel
    UINT32      BSum;               ///< Blue sum of the pixel
    union
    {
        UINT32  GRSum;              ///< GreenRed sum of the pixel
        UINT32  GSum;               ///< Green sum of the pixel
    };
    union
    {
        UINT32  GBSum;              ///< GreenBlue sum of the pixel
        UINT32  YSum;               ///< Y sum of the pixel
    };

    UINT16      RCount;             ///< Red count of the pixel
    UINT16      BCount;             ///< Blue count of the pixel
    union
    {
        UINT16  GRCount;            ///< GreenRed count of the pixel
        UINT16  GCount;             ///< Green count of the pixel
    };
    union
    {
        UINT16  GBCount;            ///< GreenBlue count of the pixel
        UINT16  YCount;             ///< Y count of the pixel
    };

} StatsBayerGridChannelInfo;

///< @brief Defines statistic data for each color channel including saturation
typedef struct StatsBayerGridChannelInfoS_Sat: public StatsBayerGridChannelInfo
{

    UINT32      saturatedRSum;      ///< Red saturated sum of the pixel
    UINT32      saturatedBSum;      ///< Blue saturated sum of the pixel
    UINT32      saturatedGRSum;     ///< GreenRed saturated sum of the pixel
    UINT32      saturatedGBSum;     ///< GreenBlue saturated sum of the pixel

    UINT16      saturatedRCount;    ///< Red saturated count of the pixel
    UINT16      saturatedBCount;    ///< Blue saturated count of the pixel
    UINT16      saturatedGRCount;   ///< GreenRed saturated count of the pixel
    UINT16      saturatedGBCount;   ///< GreenBlue saturated count of the pixel

} StatsBayerGridChannelInfoWithSat;

///< @brief Defines the Bayer Grid statistic data structure
typedef struct
{
    UINT32*                     pGridData[StatisticsBayerChannelsCount];    ///< Bayer grid channel data from sensor HDR stream
    StatsColorChannelType       gridDataType[StatisticsBayerChannelsCount]; ///< Indicate each channel's color type
    UINT32                      horizontalRegionCount;                      ///< The number of regions in horizontal
    UINT32                      verticalRegionCount;                        ///< The number of regions in vertial
    UINT32                      totalRegionCount;                           ///< The total number of regions
    UINT32                      regionWidth;                                ///< The width of the region
    UINT32                      regionHeight;                               ///< The height of the region
    UINT16                      bitDepth;                                   ///< The bit depth for each value in the region

    struct
    {
        UINT32      usesY       : 1;
        UINT32      hasSatInfo  : 1;
        UINT32      onlyY       : 1;
        UINT32      reserved    : 29;
    } flags;

    UINT32                      satThreshold[StatisticsBayerChannelsCount]; ///< The threshold for saturation

    UINT32                      numValidRegions;                            ///< The valid region count

    inline VOID SetChannelDataArray(StatsBayerGridChannelInfo *pChannelInfo)
    {
        m_pChannelData = pChannelInfo;
    }

    inline StatsBayerGridChannelInfo* GetChannelData(UINT32 index)
    {
        if (NULL == m_pChannelData)
        {
            return NULL;
        }
        StatsBayerGridChannelInfoWithSat *pChannelSat = reinterpret_cast <StatsBayerGridChannelInfoWithSat*>(m_pChannelData);
        StatsBayerGridChannelInfo* pChannelInfo = flags.hasSatInfo ? (pChannelSat + index) : (m_pChannelData + index);
        return pChannelInfo;
    }

    StatisticsStartUp           startupMode;        ///< statistics start up mode.
private:
    StatsBayerGridChannelInfo*  m_pChannelData;     ///< Stats bayer grid channel data
} StatsBayerGrid;

///< @brief Defines the Bayer Histogram statistic data structure
typedef struct
{
    UINT32                  channelCount;                               ///< Indicate the number of color channels in the
                                                                        ///  statistic
    const UINT32*           pHistData[StatisticsBayerChannelsCount];    ///< The actual histogram data in each channel
    StatsColorChannelType   histDataType[StatisticsBayerChannelsCount]; ///< Indicate each channel's color type
    UINT32                  binCount;                                   ///< The bin count for the histogram
    StatisticsStartUp       startupMode;                                ///< statistics start up mode.
} StatsBayerHist;

///< @brief Defines the measurement region type
typedef enum
{
    GlobalRegion,                                                       ///< stats is measured by full frame region
    ROIWindowRegion                                                     ///< statistics data is measured by specific ROI window
}MeasurementRegionType;

///< @brief Defines the type of statistis data measurement
typedef enum
{
    HDR3ExpSplitLong,                                                   ///< statitics data is measured by long exposure pixel
    HDR3ExpSplitMiddle,                                                 ///< statitics data is measured by middle exposure pixel
    HDR3ExpSplitShort,                                                  ///< statitics data is measured by short exposure pixel
    HDR3ExpMerged                                                       ///< statitics data is measured by long/middle/short exposure pixel
                                                                        ///  pixel after merged
}HDR3ExposureMergedType;

///< @brief Defines the 3-Exposure HDR statistic sub histogram data structure
typedef struct
{
    UINT32*                 pHistData[StatisticsBayerChannelsCount];             ///< The actual histogram data in each channel
    StatsColorChannelType   histDataType[StatisticsBayerChannelsCount];          ///< Indicate each channel's color type
    UINT32                  binCount;                                            ///< The bin count for the histogram
    UINT32                  totalHistCount;                                      ///< total count for all histogram bin
    UINT16                  histWeightUnit;                                      ///< Histogram weight
    MeasurementRegionType   measurementRegion;                                   ///< Meaturement region
    StatsRectangle          measurementRegionCoordinate;                         ///< Measurement region coordinate
    HDR3ExposureMergedType  mergedType;                                          ///< merged or slipt long/middle/short expsoure
}StatsHDR3ExposureHistStats;

///< @brief Defines the 3-Exposure HDR statistic sub data structure
typedef struct
{
    StatsHDR3ExposureHistStats      splitBayerHist;                             ///< RDI split histogram stats
    UINT32                          splitHistchannelCount;                      ///< Indicate the number of channels in the
                                                                                ///  split histogram data
    StatsHDR3ExposureHistStats      mergedBayerHist;                            ///< RDI merged histogram stats
    UINT32                          mergedHistchannelCount;                     ///< Indicate the number of channels in the
                                                                                ///  merged histogram data
    StatsBayerGrid                  gridBayerStats;                             ///< RDI grid bayer stats
    StatsHDR3ExposureHistStats      ROIBayerHist;                               ///< RDI ROI bayer stats
} StatsHDR3ExposureDataType;

/// @brief This enumeration represents for 3HDR type
typedef enum
{
    HDR3ExposureTypeUnknown = 0,          ///< Invalid 3ExposureHDR type
    HDR3ExposureType1,                    ///< 3ExposureHDR type1, with split histogram data and merged histogram data
    HDR3ExposureType2,                    ///< 3ExposureHDR type2, with split histogram data, merged histogram data
                                          ///  and thumbnail(grid) statistics data
    HDR3ExposureType3,                    ///< 3ExposureHDR type3, with split histogram data, merged histogram data
                                          ///  and ROI related statistics data
}HDR3ExposureType;

///< @brief Defines the 3HDR statistic data structure
typedef struct
{
    const StatsHDR3ExposureDataType* pHDR3ExposureStats;                 ///< 3-Exposure HDR stats data
    HDR3ExposureType                 HDR3ExposureType;                   ///< 3-Exposure HDR type
    StatisticsStartUp                startupMode;                        ///< statistics start up mode.
}HDR3ExposureStats;

///< @brief Defines the IHistogram statistic Channel Type
enum StatsIHistChannelType
{
    IHistChannelHISTOGRAM = 0,          ///< IHistogram Single channel legacy
    IHistChannelR,                      ///< IHistogram Channel Red
    IHistChannelB,                      ///< IHistogram Channel Blue
    IHistChannelG,                      ///< IHistogram Channel Green
    IHistChannelYCC,                    ///< IHistogram Channel Luma
    IHistChannelCount,                  ///< Count of the items in the definition
    IHistChannelMax     = 0x7FFFFFFF    ///< Anchor to indicate the last item in the definition
};

///< @brief Defins the IHistogram statistic data structure
struct StatsIHist
{
    UINT16            arraySize;                        ///< Number of elements in the channelData field
    UINT16            numBins;                          ///< Number of bins for which the histogram is collected
    UINT32            validChannelsMask;                ///< Flag to indicate which channel is valid. Each bit corresponds to
                                                        ///  one channel, with the first channel represented by the LSB bit
    UINT32*           channelData[IHistChannelCount];   ///< Channel data for each channel
};

///< @brief Defines the format of the color correction matrix
struct StatsAWBCCM
{
    BOOL        isCCMOverrideEnabled;                       ///< Flag indicates if CCM override is enabled.
    FLOAT       CCM[StatsCCMRowCount][StatsCCMColCount];    ///< The color correction matrix
    FLOAT       CCMOffset[StatsCCMRowCount];                ///< The offsets for color correction matrix
};

///< @brief Defines the format of the color correction matrix
struct StatsAWBCCMList
{
    UINT32          numValidCCMs;        ///< Number of Valid CCMs
    StatsAWBCCM     CCM[MaxCCM];         ///< Color Correction Matrix Value
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This enumeration represents what HDR mode stats are collected for.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    ExposureInvalid = -1,           ///< Invalid exposure
    ExposureAll,                    ///< All exposure
    ExposureLong,                   ///< Long exposure
    ExposureShort,                  ///< Short exposure
    ExposureMax     = 0x7FFFFFFF    ///< Anchor to indicate the last item in the defines
}StatisticsHDRMode;

/// @brief Structure describing luma sum
typedef struct
{
    UINT32  lumaSumCount;                   ///< Number of valid entries in the lumaSum array
    FLOAT   lumaSum[StatisticsYStatsNum];   ///< Luma sum array
} StatsLumaSum;

/// @brief Structure describing a weighted region of interest
typedef struct
{
    StatsRectangle  ROI;        ///< Region of interest
    UINT32          ROIWeight;  ///< Weight corresponding to region of interest
} StatsWeightedROI;

/// @brief Represents a standard OS timestamp including two components
typedef struct
{
  UINT64 timeStampNanoSecond;   ///< The time since bootup in nanoseconds
} StatsTimeStamp;

/// @brief Vendor tag definition. This structure should never be modified. It will break the backward compatiblity otherwise.
typedef struct
{
    UINT32  vendorTagId;    ///< Vendor tag identification number
    UINT32  appliedDelay;   ///< Specifies which request this vendor tag is targeted for
} StatsVendorTagInfo;

/// @brief Vendor tag definition. This structure should never be modified. It will break the backward compatiblity otherwise.
typedef struct
{
    UINT32  vendorTagId;        ///< Vendor tag identification number
    VOID*   pData;              ///< Pointer to the vendor data
    UINT32  dataSize;           ///< Size in bytes of the data
    UINT32  sizeOfWrittenData;  ///< Size of written data by OEM in bytes
    INT32   appliedDelay;       ///< Specifies the delay relative to current frame
} StatsVendorTag;

/// @brief Defines statistics vendor tag list information of depending vendor tags
typedef struct
{
    UINT32              vendorTagCount;                     ///< Vendor tag count filled-in
    StatsVendorTagInfo  vendorTag[StatisticsMaxVendorTag];  ///< Vendor tag details
} StatsVendorTagInfoList;

/// @brief Defines statistics vendor tag list information
typedef struct
{
    UINT32          vendorTagCount;                     ///< Vendor tag count filled-in
    StatsVendorTag  vendorTag[StatisticsMaxVendorTag];  ///< Vendor tag details
} StatsVendorTagList;

/// @brief Defines structure for the gyro data
typedef struct
{
    FLOAT   x;  ///< Rate of rotation around the x axis
    FLOAT   y;  ///< Rate of rotation around the y axis
    FLOAT   z;  ///< Rate of rotation around the z axis
} StatsGyroInfo;

/// @brief Defines structure for the statistics algorithm role
typedef enum
{
    StatsAlgoRoleDefault,           ///< The stats algorithm runs in regular way
    StatsAlgoRoleMaster,            ///< The Stats algorithm runs as master
    StatsAlgoRoleSlave,             ///< The Stats algorithm runs as slave
    StatsAlgoRoleMax                ///< The Stats algorithm role max
}StatsAlgoRole;

/// @brief Defines structure for the statistics camera type
typedef enum
{
    StatsCameraTypeDefault,         ///< Default camera type
    StatsCameraTypeWide,            ///< Wideangle camera type
    StatsCameraTypeTele,            ///< Telescope camera type
    StatsCameraTypeVR,              ///< 360 VR camera type
    StatsCameraTypeMax              ///< Camera type max
}StatsCameraType;

/// @brief Defines structure for the statistics camera information
typedef struct
{
    StatsAlgoRole           algoRole;           ///< The role that statistics algorithm runs (Master/Slave)
    UINT32                  cameraId;           ///< The camera ID
    StatsCameraType         cameraType;         ///< The camera type (Wide/Tele)
}StatsCameraInfo;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHISTATSINTERFACEDEFS_H
