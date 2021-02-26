////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chiispstatsdefs.h
///
/// @brief Qualcomm Technologies, Inc. 3A/ISP Stats definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHIISPSTATSDEFS_H
#define CHIISPSTATSDEFS_H

#include "chistatsproperty.h"
#include "camxdefs.h"
#include "camxcdktypes.h"

#ifdef __cplusplus

extern "C"
{
#endif // __cplusplus

/// @brief Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

/// @todo (CAMX-1838) move hardware dependent parameters from common header
/// @brief BHist HW data
static const UINT32 MaxBHistBinNum          = 1024;     ///< Max stats number for Bayer Histogram
static const UINT32 StatsBHistBinSize       = 4;        ///< Max Bayer Histogram bin size
static const UINT32 MaxBHistChannelNum      = 4;        ///< Max channel number for bhist

/// @brief AWB BG HW data
static const UINT32 MaxAWBBGStatsNum        = 3888;    ///< Max ROIs for AWB BG is 72x54,
                                                       ///  usually we only use 64x48

/// @brief tintless BG HW data
static const UINT32 MaxTintlessBGStatsNum   = 3072;      ///< Max ROIs for Tintless BG is 64x48

/// @brief HDRBE HW data
static const UINT32 MaxHDRBEStatsNum        = 3072;     ///< Max ROIs for HDR BE is 64x48

/// @brief IHist HW data
static const UINT32 NumIHistBins            = 256;      ///< Number of Bins in Image Histogram

/// @brief HDRBHist HW data
static const UINT32 MaxHDRBHistChannelNum   = 3;        ///< Max channel number for HDR BHist
static const UINT32 NumHDRBHistBins         = 256;      ///< Number of Bins used in HDR Bayer Histogram
static const UINT32 StatsHDRBHistBinSize    = 4;        ///< Max HDR Bayer Histogram bin size (bytes)

/// @brief RS & CS HW data
static const UINT32 MaxCSHorizRegions       = 1560;     ///< Max number of CS horizontal regions
static const UINT32 MaxCSVertRegions        = 4;        ///< Max number of CS vertical regions
static const UINT32 MaxRSHorizRegions       = 16;       ///< Maximum number of RS Horizontal regions
static const UINT32 MaxRSVertRegions        = 1024;     ///< Maximum number of RS vertical regions

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This enumeration represents what type of stats are collected.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    ISPStatsTypeUndefined,       ///< Undefined
    ISPStatsTypeHDRBE,           ///< HDR BE stats
    ISPStatsTypeAWBBG,           ///< AWB BG stats
    ISPStatsTypeTintlessBG,      ///< Tintless BG stats
    ISPStatsTypeBF,              ///< BF stats
    ISPStatsTypeHDRBHist,        ///< HDR BHistogram stats
    ISPStatsTypeIHist,           ///< Image Histogram stats
    ISPStatsTypeBHist,           ///< BHistogram stats
    ISPStatsTypeRS,              ///< RS stats
    ISPStatsTypeCS,              ///< CS stats
    ISPStatsTypeRDIPDAF,         ///< RDIPDAF
    ISPStatsTypeBPSAWBBG,        ///< BPS AWB BG stats
    ISPStatsTypeRDIStats,        ///< RDI stats
    ISPStatsTypeLongHDRBE,       ///< Long HDR BE stats
    ISPStatsTypeLongAWBBG,	     ///< AWB BG stats
    ISPStatsTypeLongTintlessBG,  ///< Tintless BG stats
    ISPStatsTypeLongBF,          ///< BF stats
    ISPStatsTypeLongHDRBHist,    ///< HDR BHistogram stats
    ISPStatsTypeLongIHist,       ///< Image Histogram stats
    ISPStatsTypeLongBHist,       ///< BHistogram stats
    ISPStatsTypeLongRS,          ///< RS stats
    ISPStatsTypeLongCS,          ///< CS stats
    ISPStatsTypeBPSRegYUV,       ///< YUV frame to be processed in StatsParser
} ISPStatsType;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This enumeration represents which pixels to be used for HDR mode stats collection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    StatsHDRModeExposureAll,      ///< All pixels
    StatsHDRModeExposureLong,     ///< Long pixels
    StatsHDRModeExposureShort,    ///< Short pixels
} StatsHDRMode;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This enumeration represents id saturated stats are collected.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    BGExtendedFieldsNone,         ///< No saturation
    BGExtendedFieldsSaturated,    ///< Saturated
} BGExtendedFields;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This structure is used to pass the calculated Bayer grid statistics from the ISP.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    UINT32 regionHorizNum;                             ///< Horizontal number of regions for the BG. 64, max 72.
    UINT32 regionVertNum;                              ///< Vertical number of regions for the BG. 48, max 54.
    UINT32 regionPixelCnt;                             ///< The count of the region pixels.
    UINT32 regionHeight;                               ///< The height of each region.
    UINT32 regionWidth;                                ///< The width of each region.
    UINT16 redMax;                                     ///< The max for red channel.
    UINT16 blueMax;                                    ///< The max for blue channel.
    UINT16 greenRedMax;                                ///< The max for green-red channel.
    UINT16 greenBlueMax;                               ///< The max for green-blue channel.
    UINT32 redSum[MaxAWBBGStatsNum];                   ///< Array with the sums for the red pixels.
    UINT32 blueSum[MaxAWBBGStatsNum];                  ///< Array with the sums for the blue pixels.
    UINT32 greenRedSum[MaxAWBBGStatsNum];              ///< Array with the sums for the green-red pixels.
    UINT32 greenBlueSum[MaxAWBBGStatsNum];             ///< Array with the sums for the green-blue pixels.
    UINT32 redNum[MaxAWBBGStatsNum];                   ///< Array with the number of the red pixels.
    UINT32 blueNum[MaxAWBBGStatsNum];                  ///< Array with the number of the blue pixels.
    UINT32 greenRedNum[MaxAWBBGStatsNum];              ///< Array with the number of the green-red pixels.
    UINT32 greenBlueNum[MaxAWBBGStatsNum];             ///< Array with the number of the green-blue pixels.

    UINT32 redSaturatedSum[MaxAWBBGStatsNum];          ///< Array with the saturated sums for the red pixels.
    UINT32 blueSaturatedSum[MaxAWBBGStatsNum];         ///< Array with the saturated sums for the blue pixels.
    UINT32 greenRedSaturatedSum[MaxAWBBGStatsNum];     ///< Array with the saturated sums for the green-red pixels.
    UINT32 greenBlueSaturatedSum[MaxAWBBGStatsNum];    ///< Array with the saturated sums for the green-blue pixels.
    UINT32 redSaturatredNum[MaxAWBBGStatsNum];         ///< Array with the number of the saturated red pixels.
    UINT32 blueSaturatedNum[MaxAWBBGStatsNum];         ///< Array with the number of the saturated blue pixels.
    UINT32 greenRedSaturatedNum[MaxAWBBGStatsNum];     ///< Array with the number of the saturated green-red pixels.
    UINT32 greenBlueSaturatedNum[MaxAWBBGStatsNum];    ///< Array with the number of the saturated green-blue pixels.

    UINT32 extendedFields;                             ///< Represents whether certain extended fields are present.
                                                       ///  BGExtendedFields lists the possible supported fields.
} BGStatsOutput;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief this structure is used to pass the calculated Bayer grid statistics from the ISP.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    ColorChannel    channelType;                        ///< the channel type
    UINT32          numBins;                            ///< Num of bins containing valid data
    UINT32          BHistogramStats[MaxBHistBinNum];    ///< Array containing the histogram values
    UINT64          requestID;                          ///< frame for which stats were generated
} ParsedBHistStatsOutput;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Image histogram statistics data
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    UINT16 YCCHistogram[NumIHistBins];      ///< Array containing the either the Y, Cb or Cr histogram values
    UINT16 greenHistogram[NumIHistBins];    ///< Array containing the green histogram values
    UINT16 blueHistogram[NumIHistBins];     ///< Array containing the blue histogram values
    UINT16 redHistogram[NumIHistBins];      ///< Array containing the red histogram values
} IHistStatsOutput;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This structure is used to pass the parsed stats
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    IHistStatsOutput    imageHistogram; ///< Image histogram statistics data
    UINT32              numBins;        ///< Number of bins per channel
} ParsedIHistStatsOutput;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief HDR Bayer Histogram statistics data
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    UINT32  redHistogram[NumHDRBHistBins];      ///< Array containing the red histogram values.
    UINT32  greenHistogram[NumHDRBHistBins];    ///< Array containing the green histogram values.
    UINT32  blueHistogram[NumHDRBHistBins];     ///< Array containing the blue histogram values.
} HDRBHistStatsOutput;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This structure is used to pass the parsed HDR Bayer Histogram (HDRBHist) stats from stats parser.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    HDRBHistStatsOutput HDRBHistStats;  ///< HDR Bayer Histogram statistics data
    UINT32              numBins;        ///< Number of bins per channel
} ParsedHDRBHistStatsOutput;

/// @brief This structure is populated with per channel parsed AWB BG stats non-saturation information.
typedef struct
{

    UINT32      RSum;           ///< Red sum of the pixel
    UINT32      BSum;           ///< Blue sum of the pixel
    union
    {
        UINT32  GRSum;          ///< GreenRed sum of the pixel
        UINT32  GSum;           ///< Green sum of the pixel
    };
    union
    {
        UINT32  GBSum;          ///< GreenBlue sum of the pixel
        UINT32  YSum;           ///< Y sum of the pixel
    };

    UINT16      RCount;         ///< Red count of the pixel
    UINT16      BCount;         ///< Blue count of the pixel
    union
    {
        UINT16  GRCount;        ///< GreenRed count of the pixel
        UINT16  GCount;         ///< Green count of the pixel
    };
    union
    {
        UINT16  GBCount;        ///< GreenBlue count of the pixel
        UINT16  YCount;         ///< Y count of the pixel
    };

} AWBBGChannelData;

/// @brief This structure is populated with per channel parsed AWB BG stats with saturation information.

typedef struct AWBBGChannelDataWithSat_S : public AWBBGChannelData
{
    UINT32    saturatedRSum;    ///< Red saturated sum of the pixel
    UINT32    saturatedBSum;    ///< Blue saturated sum of the pixel
    UINT32    saturatedGRSum;   ///< GreenRed saturated sum of the pixel
    UINT32    saturatedGBSum;   ///< GreenBlue saturated sum of the pixel

    UINT16    saturatedRCount;  ///< Red saturated count of the pixel
    UINT16    saturatedBCount;  ///< Blue saturated count of the pixel
    UINT16    saturatedGRCount; ///< GreenRed saturated count of the pixel
    UINT16    saturatedGBCount; ///< GreenBlue saturated count of the pixel
} AWBBGChannelDataWithSat;

/// @brief This structure is used to pass the parsed AWB Bayer Grid (AWB BG) stats from stats parser.
typedef struct
{
    struct
    {
        UINT32      usesY       : 1;
        UINT32      hasSatInfo  : 1;
        UINT32      reserved    : 30;
    } flags;

    UINT32              numROIs;                          ///< number of valid entries

    inline AWBBGChannelData GetChannelData(UINT32 index, UINT32 hasSatInfo)
    {
        if (hasSatInfo)
        {
            return  m_channelDataWithSat[index];
        }
        else
        {
            return m_channelData[index];
        }
    }

    inline AWBBGChannelData GetChannelData(UINT32 index)
    {
        if (flags.hasSatInfo)
        {
            return  m_channelDataWithSat[index];
        }
        else
        {
            return m_channelData[index];
        }
    }

    inline VOID * GetChannelDataArray()
    {
        return &m_channelData;
    }

    inline VOID SetChannelData(AWBBGChannelData In, UINT32 index, UINT32 hasSatInfo)
    {
        if (hasSatInfo)
        {
            AWBBGChannelDataWithSat *Ptr = reinterpret_cast<AWBBGChannelDataWithSat*>(&In);
            m_channelDataWithSat[index] = *Ptr;
        }
        else
        {
            AWBBGChannelData *Ptr = reinterpret_cast<AWBBGChannelData*>(&In);
            m_channelData[index] = *Ptr;
        }
    }

private:
    union {
      AWBBGChannelData           m_channelData[MaxAWBBGStatsNum];           ///< the stats data
      AWBBGChannelDataWithSat    m_channelDataWithSat[MaxAWBBGStatsNum];    ///< the stats data
    };
} ParsedAWBBGStatsOutput;

CAMX_BEGIN_PACKED

/// @brief This structure is used to pass the Tintless less BG non-saturation stats per Region.
typedef struct TintlessBGStatsInfo
{
    UINT32 RSum; ///< RSum of Tintless BG Stats
    UINT32 BSum; ///< BSum of Tintless BG Stats

    union
    {
        UINT32  GRSum; ///< GreenRed sum of the pixel
        UINT32  GSum;  ///< Green sum of the pixel
    };

    union
    {
        UINT32  GBSum; ///< GreenBlue sum of the pixel
        UINT32  YSum;  ///< Y sum of the pixel
    };

    UINT16      RCount; ///< Red count of the pixel
    UINT16      BCount; ///< Blue count of the pixel

    union
    {
        UINT16  GRCount; ///< GreenRed count of the pixel
        UINT16  GCount;  ///< Green count of the pixel
    };

    union
    {
        UINT16  GBCount; ///< GreenBlue count of the pixel
        UINT16  YCount;  ///< Y count of the pixel
    };
} CAMX_PACKED TintlessBGStatsInfo;

/// @brief This structure is used to pass the Tintless less BG stats with saturation per Region.
typedef struct TintlessBGStatsInfoWithSat : public TintlessBGStatsInfo
{
    UINT32 SaturationRSum;  ///< Red saturated sum of the pixel
    UINT32 SaturationBSum;  ///< Blue saturated sum of the pixel
    UINT32 SaturationGRSum; ///< GreenRed saturated sum of the pixel
    UINT32 SaturationGBSum; ///< GreenBlue saturated sum of the pixel

    UINT16 SaturationRCount;  ///< Red saturated count of the pixel
    UINT16 SaturationBCount;  ///< Blue saturated count of the pixel
    UINT16 SaturationGRCount; ///< GreenRed saturated count of the pixel
    UINT16 SaturationGBCount; ///< GreenBlue saturated count of the pixel
} CAMX_PACKED TintlessBGStatsInfoWithSat;

CAMX_END_PACKED

/// @brief This structure is used to pass the parsed Tintless Bayer Grid (TintlessBG) stats from stats parser.
class ParsedTintlessBGStatsOutput
{
public:
    struct
    {
        UINT32 hasYStats           : 1;  ///< Has Y channel Stats
        UINT32 hasSaturationPixels : 1;  ///< Has Saturated Pixels
        UINT32 reserved            : 30; ///< Reserved Fileds
    } m_flags;

    UINT32  m_numOfRegions;                ///< Total number of stats regions
    UINT32  m_statsBitWidth;               ///< Stats Bit Width
    UINT32  m_channelGainThreshold[ChannelIndexCount];  //< Channel Gain Threshold

    inline const TintlessBGStatsInfo* GetTintlessBGStatsInfo(UINT32 index) const
    {
        if (0 == m_flags.hasSaturationPixels)
        {
            return &m_stats[index];
        }
        else
        {
            return &m_statsWithSat[index];
        }
    }

    inline TintlessBGStatsInfo* GetMutableTintlessBGStatsInfo(UINT32 index)
    {
        if (0 == m_flags.hasSaturationPixels)
        {
            return &m_stats[index];
        }
        else
        {
            return &m_statsWithSat[index];
        }
    }

    inline UINT32 GetChannelGainThreshold(UINT32 index)
    {
        return m_channelGainThreshold[index];
    }

    inline VOID SetChannelGainThreshold(const UINT32 channelGainThreshold[])
    {
        m_channelGainThreshold[ChannelIndexR] = channelGainThreshold[ChannelIndexR];
        m_channelGainThreshold[ChannelIndexB] = channelGainThreshold[ChannelIndexB];
        m_channelGainThreshold[ChannelIndexGR] = channelGainThreshold[ChannelIndexGR];
        m_channelGainThreshold[ChannelIndexGB] = channelGainThreshold[ChannelIndexGB];
    }

    inline UINT32 GetStatsBitWidth()
    {
        return m_statsBitWidth;
    }

    inline VOID SetStatsBitWidth(UINT32 bitWidth)
    {
        m_statsBitWidth = bitWidth;
    }

    inline VOID* GetTintlessBGStatsInfoArray()
    {
        return m_stats;
    }

    inline VOID SetTintlessBGStatsInfo(UINT32 index, const TintlessBGStatsInfo* in)
    {
        if (NULL != in)
        {
            if (0 == m_flags.hasSaturationPixels)
            {
                m_stats[index] = *in;
            }
            else
            {
                m_statsWithSat[index] = *(reinterpret_cast<const TintlessBGStatsInfoWithSat*>(in));
            }
        }
    }

private:
    union
    {
        TintlessBGStatsInfoWithSat  m_statsWithSat[MaxTintlessBGStatsNum]; ///< Array of TintlessBG saturation stats Info
        TintlessBGStatsInfo         m_stats[MaxTintlessBGStatsNum];        ///< Array of TintlessBG full stats info
    };
};

/// @brief This structure is populated with per channel parsed HDR BE stats non-saturation information.
typedef struct
{
    UINT32      RSum;           ///< Red sum of the pixel
    UINT32      BSum;           ///< Blue sum of the pixel
    union
    {
        UINT32  GRSum;          ///< GreenRed sum of the pixel
        UINT32  GSum;           ///< Green sum of the pixel
    };
    union
    {
        UINT32  GBSum;          ///< GreenBlue sum of the pixel
        UINT32  YSum;           ///< Y sum of the pixel
    };

    UINT16      RCount;         ///< Red count of the pixel
    UINT16      BCount;         ///< Blue count of the pixel
    union
    {
        UINT16  GRCount;        ///< GreenRed count of the pixel
        UINT16  GCount;         ///< Green count of the pixel
    };
    union
    {
        UINT16    GBCount;      ///< GreenBlue count of the pixel
        UINT16    YCount;       ///< Y count of the pixel
    };

} HDRBEChannelData;

/// @brief This structure is populated with per channel parsed HDR BE stats with saturation information.
typedef struct HDRBEChannelDataWithSat_S : public HDRBEChannelData
{
    UINT32    saturatedRSum;    ///< Red saturated sum of the pixel
    UINT32    saturatedBSum;    ///< Blue saturated sum of the pixel
    UINT32    saturatedGRSum;   ///< GreenRed saturated sum of the pixel
    UINT32    saturatedGBSum;   ///< GreenBlue saturated sum of the pixel

    UINT16    saturatedRCount;  ///< Red saturated count of the pixel
    UINT16    saturatedBCount;  ///< Blue saturated count of the pixel
    UINT16    saturatedGRCount; ///< GreenRed saturated count of the pixel
    UINT16    saturatedGBCount; ///< GreenBlue saturated count of the pixel

} HDRBEChannelDataWithSat;

/// @brief This structure is used to pass the calculated HDR Bayer Exposure (HDR BE) statistics from the ISP.
typedef struct
{
    struct
    {
        UINT32      usesY       : 1;
        UINT32      hasSatInfo  : 1;
        UINT32      onlyY       : 1;
        UINT32      reserved    : 29;
    } flags;

    UINT32              numROIs;                          ///< number of valid entries

    inline HDRBEChannelData  GetChannelData(UINT32 index, UINT32 hasSatInfo)
    {
        if (hasSatInfo)
        {
            return  m_channelDataWithSat[index];
        }
        else
        {
            return m_channelData[index];
        }
    }

    inline HDRBEChannelData GetChannelData(UINT32 index)
    {
        if (flags.hasSatInfo)
        {
            return  m_channelDataWithSat[index];
        }
        else
        {
            return m_channelData[index];
        }
    }

    inline VOID * GetChannelDataArray()
    {
        return &m_channelData;
    }

    inline VOID SetChannelData(HDRBEChannelData In, UINT32 index, UINT32 hasSatInfo)
    {
        if (hasSatInfo)
        {
            HDRBEChannelDataWithSat *Ptr = reinterpret_cast<HDRBEChannelDataWithSat*>(&In);
            m_channelDataWithSat[index] = *Ptr;
        }
        else
        {
            HDRBEChannelData *Ptr = reinterpret_cast<HDRBEChannelData*>(&In);
            m_channelData[index] = *Ptr;
        }
    }
    inline VOID SetYChannelData(float y_avg, UINT32 index)
    {
        m_channelData[index].YSum = static_cast<UINT>(y_avg);
        m_channelData[index].YCount = 1;
        m_channelData[index].RCount = 0;
        m_channelData[index].GCount = 0;
        m_channelData[index].BCount = 0;
    }

private:
    union {
      HDRBEChannelData           m_channelData[MaxHDRBEStatsNum];           ///< the stats data
      HDRBEChannelDataWithSat    m_channelDataWithSat[MaxHDRBEStatsNum];    ///< the stats data
    };
} ParsedHDRBEStatsOutput;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This structure is used to pass the calculated Bayer Focus (BF) statistics from the ISP.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    UINT64                  frameTag;                                 ///< 64-bit frame-tag programmed by SW for the region
    UINT64                  horizontal1Sum[BFMaxROIRegions];          ///< Sum of pixels in H1 filter
    UINT64                  horizontal2Sum[BFMaxROIRegions];          ///< Sum of pixels in H2 filter
    UINT64                  verticalSum[BFMaxROIRegions];             ///< Sum of pixels in V filter
    UINT64                  horizontal1Sharpness[BFMaxROIRegions];    ///< Sharpness of pixels in H1 filter
    UINT64                  horizontal2Sharpness[BFMaxROIRegions];    ///< Sharpness of pixels in H2 filter
    UINT64                  verticalSharpness[BFMaxROIRegions];       ///< Sharpness of pixels in V filter
    UINT32                  horizontal1Num[BFMaxROIRegions];          ///< Number of pixels in H1 filter
    UINT32                  horizontal2Num[BFMaxROIRegions];          ///< Number of pixels in H2 filter
    UINT32                  verticalNum[BFMaxROIRegions];             ///< Number of pixels in V filter
    UINT32                  regionID[BFMaxROIRegions];                ///< Region ID
    UINT32                  region[BFMaxROIRegions];                  ///< Region is the primary/secondary
    UINT32                  numOfROIRegions;                          ///< Number of BF ROI regions
} ParsedBFStatsOutput;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This structure is used to pass the calculated Column Sum (CS) statistics from the ISP.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ParsedCSStatsOutput
{
    UINT64  frameTag;                                       ///< 64-bit frame - tag programmed by SW for the region
    UINT32  columnSum[MaxCSHorizRegions][MaxCSVertRegions]; ///< Array containing column sum of pixel values
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This structure is used to pass the calculated Row Sum (RS) statistics from the ISP.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ParsedRSStatsOutput
{
    UINT64  frameTag;                                       ///< 64-bit frame - tag programmed by SW for the region
    UINT32  rowSum[MaxRSHorizRegions][MaxRSVertRegions];    ///< Array containing row sum of pixel values
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief this structure is used to pass the region statistics from the ISP stats modules.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    BOOL    isValid;              ///< Is this valid
    UINT32  horizRegionsStart;    ///< Start of the horizontal region
    UINT32  horizRegionsEnd;      ///< End of the horizontal region
    UINT32  horizRegionsTotal;    ///< Number of horizontal regions
    UINT32  vertRegionsTotal;     ///< Number of vertical regions
} ISPRegionsStatsParams;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief this structure is used to pass the ROI statistics from the ISP stats modules.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    UINT32 regionWidth;          ///< Region width
    UINT32 regionHeight;         ///< Region height
    UINT32 regionHorizOffset;    ///< Region horizontal offset
    UINT32 regionVertOffset;     ///< Region vertical offset
    UINT32 regionHorizNum;       ///< Number of horizontal regions
    UINT32 regionVertNum;        ///< Number of vertical regions
    UINT16 redMax;               ///< max R
    UINT16 greenRedMax;          ///< max Gr
    UINT16 blueMax;              ///< max B
    UINT16 greenBlueMax;         ///< max Gb
} ISPStatsROIParams;

/// @brief Structure to pass the AWB BG configuration to ISP and write back if any adjustment made by ISP
typedef struct
{
    BGBEConfig  AWBBGConfig;    ///< AWB BG configuration from stats and this can be overwritten if the requested
                                ///  configuration cannot be fulfilled in the case of hardware limitations or certain
                                ///  conflicting requirements.
    BOOL        isAdjusted;     ///< Indicate if IFE made any adjustment for the requested configuration data.
    UINT32      regionWidth;    ///< AWB individual region width
    UINT32      regionHeight;   ///< AWB individual region height
} ISPAWBBGStatsConfig;

/// @brief Structure to pass the HDR BE configuration to ISP and write back if any adjustment made be ISP
typedef struct
{
    BGBEConfig  HDRBEConfig;    ///< HDR BE configuration from stats and this can be overwritten if the requested
                                ///  configuration cannot be fulfilled in the case of hardware limitations or certain
                                ///  conflicting requirements.
    BOOL        isAdjusted;     ///< Indicate if IFE made any adjustment for the requested configuration data.
    UINT32      regionWidth;    ///< HDR BE individual region width
    UINT32      regionHeight;   ///< HDR BE individual region height
} ISPHDRBEStatsConfig;

/// @brief Structure to pass the Image Histogram configuration to ISP and write back if any adjustment made by ISP
typedef struct
{
    HDRBHistConfig  HDRBHistConfig; ///< HDR BHist configuration from stats and this can be overwritten if the requested
                                    ///  configuration cannot be fulfilled in the case of hardware limitations or certain
                                    ///  conflicting requirements.
    UINT32          numBins;        ///< Number of bins in the histograms.
    BOOL            isAdjusted;     ///< Indicate if IFE made any adjustment for the requested configuration data.
} ISPHDRBHistStatsConfig;

/// @brief Structure to pass the BF configuration to ISP and write back if any adjustment made be ISP
typedef struct
{
    AFConfigParams  BFConfig;   ///< BF stats configuration from stats and this can be overwritten if the requested
                                ///  configuration cannot be fulfilled in the case of hardware limitations or certain
                                ///  conflicting requirements.
    BOOL            isAdjusted; ///< Indicate if IFE made any adjustment for the requested configuration data.
} ISPBFStatsConfig;


/// @brief Structure to pass the Image Histogram configuration to ISP and write back if any adjustment made by ISP
typedef struct
{
    IHistStatsConfig    IHistConfig;    ///< Image Histogram configuration from stats and this can be overwritten if the
                                        ///  requested configuration cannot be fulfilled in the case of hardware limitations or
                                        ///  certain conflicting requirements.
    UINT32              numBins;        ///< Number of bins in the histograms.
} ISPIHistStatsConfig;

/// @brief Structure to pass the BHist configuration to ISP and write back if any adjustment made be ISP
typedef struct
{
    BHistConfig BHistConfig;    ///< BHist stats configuration from stats and this can be overwritten if the requested
                                ///  configuration cannot be fulfilled in the case of hardware limitations or certain
                                ///  conflicting requirements.
    UINT32      numBins;        ///< Num of bins containing valid data
    BOOL        isAdjusted;     ///< Indicate if IFE made any adjustment for the requested configuration data.
    UINT64      requestID;      ///< frame for which stats were generated
} ISPBHistStatsConfig;

/// @brief Structure to pass the CS stats configuration to ISP and write back if any adjustment made by ISP
struct ISPCSStatsConfig
{
    CSStatsConfig    CSConfig;   ///< CS stats configuration
};

/// @brief Structure to pass the RS stats configuration to ISP and write back if any adjustment made be ISP
struct ISPRSStatsConfig
{
    AFDStatsRSConfig    RSConfig;   ///< RS stats configuration from stats and this can be overwritten if the requested
                                    ///  configuration cannot be fulfilled in the case of hardware limitations or certain
                                    ///  conflicting requirements.
    BOOL                isAdjusted; ///< Indicate if IFE made any adjustment for the requested configuration data.
};

/// @brief Structure to pass the TintlessBG configuration to ISP and write back if any adjustment made by ISP
typedef struct
{
    BGBEConfig tintlessBGConfig; ///< TintlessBG configuration from stats and this can be overwritten if the requested
                                 ///  configuration cannot be fulfilled in the case of hardware limitations or certain
                                 ///  conflicting requirements.
    BOOL       isAdjusted;       ///< Indicate if IFE made any adjustment for the requested configuration data.
} ISPTintlessBGStatsConfig;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHIISPSTATSDEFS_H
