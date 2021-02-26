////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxsensorselectmode.h
/// @brief SensorSelectMode class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXSENSORSELECTMODE_H
#define CHXSENSORSELECTMODE_H

#include "chxdefs.h"
#include "chi.h"

/// @brief This enumerates conditions for sensor select mode
enum SensorModeSelectCondition
{
    ConditionFrameRate = 0,          ///< Sensor frame rate
    ConditionBoundedFrameRate,       ///< Bounded frame rate
    ConditionAspectRatio,            ///< Aspect ratio
    ConditionWidth,                  ///< Resolution width
    ConditionHeight,                 ///< Resolution height
    ConditionClock,                  ///< Clock
    ConditionQuadra,                 ///< Quadra
    ConditionHFR,                    ///< High frame rate
    ConditionDEF,                    ///< DEF
    ConditionIHDR,                   ///< Interlaced HDR
    ConditionRHDR,                   ///< RHDR
    ConditionMPIX,                   ///< MPIX
    ConditionBestResolution,         ///< Best resolution match
    MaxModeSelectCondition           ///< Max
};

/// @brief This enumerates usecases for sensor select mode
enum SensorModeSelectUseCase
{
    UsecaseFastAEC = 0,     ///< Fast AEC
    UsecaseQuadra,          ///< Quadra
    UsecaseHFR,             ///< High frame rate
    UsecaseIHDR,            ///< Interlaced HDR
    UsecaseRHDR,            ///< RHDR
    UsecaseVHDR,            ///< Video HDR
    UsecaseSnapshot,        ///< Snapshot
    UsecaseVideoPreview,    ///< Video preview
    MaxModeSelectUsecase    ///< Max
};

/// @brief Structure used to select best resolution
struct SensorSelectBestResolution
{
    UINT32 temporaryResolution;         ///< Temporary resolution for matching
    UINT32 lastResolution;              ///< Last selected resolution
};

/// @brief Structure used to select best apect ratio
struct SensorSelectBestAspectRatio
{
    FLOAT lastAspectRatioDiff;        ///< Last selected aspect ratio
    FLOAT tempAspectRatioDiff;        ///< Temporary aspect ratio
};

/// @brief Sensor mode selection information
struct DesiredSensorMode
{
    UINT32             optimalWidth;    ///< Optimal width of the frame required
    UINT32             optimalHeight;   ///< Optimal height of the frame required
    UINT32             maxWidth;        ///< Max Bounded Width of the frame
    UINT32             maxHeight;       ///< Max Bounded Height of the frame
    UINT32             minWidth;        ///< Min Bounded Width of the frame
    UINT32             minHeight;       ///< Min Bounded Height of the frame
    DOUBLE             frameRate;       ///< Frame Rate of the usecase
    ChiSensorModeCaps  sensorModeCaps;  ///< List of modes
    UINT32             forceMode;       ///< uses forceMode, if it is a valid mode
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// VirtualChannel
///
/// Range:         [0,3]
/// Comments:      Virtual Channel of the data
///                Valid values for virtual channel are: 0, 1, 2 and 3
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef UINT32 VirtualChannel;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FrameDimension
/// Comments:      Frame dimension: contains xStart, yStart, width and height
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct FrameDimension
{
    UINT32 xStart;
    UINT32 yStart;
    UINT32 width;
    UINT32 height;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// StreamConfiguration
/// Comments:      Information for a stream data
///                element for vc
///                element for dt
///                element for dimension
///                element for bitWidth
///                element for type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct StreamConfiguration
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// vc
    ///
    /// Default Value: 0
    /// Range:         [0,3]
    /// Comments:      Virtual Channel of the data
    ///                Valid values for virtual channel are: 0, 1, 2 and 3
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VirtualChannel                   vc;
    /// Data type of the stream. Default value is 0x2B (10-bit RAW)
    UINT32                           dt;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// frameDimension
    /// Comments:      Frame dimension: contains xStart, yStart, width and height
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FrameDimension                   frameDimension;
    /// Bit width of the data
    UINT32                           bitWidth;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// type
    /// Default Value: IMAGE
    /// Comments:      Type of the stream
    ///                Supported stream types are: BLOB, IMAGE, PDAF, HDR, META
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CHISENSORSTREAMTYPE              type;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// StreamInformation
/// Comments:      Stream information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct StreamInformation
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// streamConfiguration - Array
    /// Min Length:    1
    /// Max Length:    4
    /// Comments:      Information for a stream data
    ///                element for vc
    ///                element for dt
    ///                element for dimension
    ///                element for bitWidth
    ///                element for type
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32               streamConfigurationCount;
    UINT32               streamConfigurationID;
    StreamConfiguration* streamConfiguration;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ResolutionData
/// Comments:      element for colorFilterArrangement
///                element for streamInfo
///                element for lineLengthPixelClock
///                element for frameLengthLines
///                element for minHorizontalBlanking
///                element for minVerticalBlanking
///                element for outputPixelClock
///                element for horizontalBinning
///                element for verticalBinning
///                element for frameRate
///                element for laneCount
///                element for settleTimeNs
///                element for is3Phase
///                element for resSettings
///                element for cropInfo
///                element for capability
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ResolutionData
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// streamInfo
    /// Comments:      Stream information
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    StreamInformation              streamInfo;
    /// Line length pixel clock of frame
    /// Typically this value is the active width + blanking width
    UINT32                         lineLengthPixelClock;
    /// Frame length lines of frame
    /// Typically this value is the active height + blanking height
    UINT32                         frameLengthLines;
    /// Minimum horizontal blanking interval in pixels
    UINT32                         minHorizontalBlanking;
    /// Minimum horizontal blanking interval in lines
    UINT32                         minVerticalBlanking;
    /// Output pixel clock
    UINT32                         outputPixelClock;
    /// Horizontal binning value
    UINT32                         horizontalBinning;
    /// Vertical binning value
    UINT32                         verticalBinning;
    /// Maximum frame rate
    DOUBLE                         frameRate;
    /// Number of lanes in which the data is streamed
    UINT32                         laneCount;
    /// Settle time in nano seconds
    UINT32                         settleTimeNs;
    /// Flag to know if the sensor is a 3phase sensor
    UINT32                         is3Phase;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// capability - Array
    /// Min Length:    1
    /// Max Length:    Unbounded
    /// Comments:      List of features / capabilities supported by sensor
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32                         capabilityCount;
    UINT32                         capabilityID;
    CHISENSORMODECAPS              capability;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ResolutionInformation
/// Comments:      Information of all the resolutions
///                Number of resolutions is detected from number of resolutionData nodes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ResolutionInformation
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// resolutionData - Array
    /// Min Length:    1
    /// Max Length:    Unbounded
    /// Comments:      element for colorFilterArrangement
    ///                element for streamInfo
    ///                element for lineLengthPixelClock
    ///                element for frameLengthLines
    ///                element for minHorizontalBlanking
    ///                element for minVerticalBlanking
    ///                element for outputPixelClock
    ///                element for horizontalBinning
    ///                element for verticalBinning
    ///                element for frameRate
    ///                element for laneCount
    ///                element for settleTimeNs
    ///                element for is3Phase
    ///                element for resSettings
    ///                element for cropInfo
    ///                element for capability
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32          resolutionDataCount;
    UINT32          resolutionDataID;
    ResolutionData* resolutionData;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Static class for selecting sensor mode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ChxSensorModeSelect
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FindBestSensorMode
    ///
    /// @brief  Select the sensor mode
    ///
    /// @param  cameraId        Camera Id
    /// @param  pSensorModeInfo Pointer to sensor mode information
    ///
    /// @return Selected sensor mode
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static ChiSensorModeInfo* FindBestSensorMode(
        UINT32                   cameraId,
        const DesiredSensorMode* pSensorModeInfo);

private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChxSensorSelectMode
    ///
    /// @brief  Private constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ChxSensorModeSelect();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChxSensorSelectMode
    ///
    /// @brief  Private copy constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ChxSensorModeSelect(const ChxSensorModeSelect& other) = delete;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ChxSensorSelectMode
    ///
    /// @brief  Private assignment operator
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ChxSensorModeSelect& operator=(const ChxSensorModeSelect& other) = delete;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsMatchingHeight
    ///
    /// @brief  Method to IsMatching height
    ///
    /// @param  index               Mode index in the binary
    /// @param  pModeInfo           Pointer to sensor mode information
    /// @param  pSensorSelectMode   Pointer to ChxSensorSelectMode
    ///
    /// @return True if height condition matches else false
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL IsMatchingHeight(
        const ChiSensorModeInfo* pModeInfo,
        const DesiredSensorMode* pSensorSelectMode);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsMatchingWidth
    ///
    /// @brief  Method to IsMatching width
    ///
    /// @param  index               Mode index in the binary
    /// @param  pModeInfo           Pointer to sensor mode information
    /// @param  pSensorSelectMode   Pointer to ChxSensorSelectMode
    ///
    /// @return True if width condition matches else false
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL IsMatchingWidth(
        const ChiSensorModeInfo* pModeInfo,
        const DesiredSensorMode* pSensorSelectMode);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsMatchingAspectRatio
    ///
    /// @brief  Method to IsMatching Aspect ratio
    ///
    /// @param  pModeInfo            Pointer to mode information in sensor driver
    /// @param  pSensorSelectMode    Pointer to SensorSelectMode as requested by application
    /// @param  pSelectedAspectRatio Pointer to selected aspect ratio
    ///
    /// @return True if aspect ratio condition matches else false
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL IsMatchingAspectRatio(
        const ChiSensorModeInfo*     pModeInfo,
        const DesiredSensorMode*     pSensorSelectMode,
        SensorSelectBestAspectRatio* pSelectedAspectRatio);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsMatchingCapability
    ///
    /// @brief  Method to IsMatching capabilities
    ///
    /// @param  index             mode index in the binary
    /// @param  pModeInfo         Pointer to sensor mode information
    /// @param  pSensorSelectMode Pointer to ChxSensorSelectMode
    ///
    /// @return True if capability condition matches else false
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL IsMatchingCapability(
        const ChiSensorModeInfo* pModeInfo,
        const DesiredSensorMode* pSensorSelectMode);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsMatchingFrameRate
    ///
    /// @brief  Method to IsMatching frame rate
    ///
    /// @param  index             mode index in the binary
    /// @param  pModeInfo         Pointer to sensor mode information
    /// @param  pSensorSelectMode Pointer to ChxSensorSelectMode
    ///
    /// @return True if frame rate condition matches else false
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL IsMatchingFrameRate(
        const ChiSensorModeInfo* pModeInfo,
        const DesiredSensorMode* pSensorSelectMode);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsMatchingBestResolution
    ///
    /// @brief  Method to IsMatching best resolution
    ///         If max resolution and best resolution are selected for a mode
    ///         preference will be given to max resolution selection
    ///
    /// @param  index               Mode index in the binary
    /// @param  pModeInfo           Pointer to mode information in sensor driver
    /// @param  pSensorSelectMode   Pointer to SensorSelectMode as requested by application
    /// @param  pMpix               Pointer to Select best sensor resolution
    ///
    /// @return True if best resolution condition matches
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL IsMatchingBestResolution(
        const ChiSensorModeInfo*    pModeInfo,
        const DesiredSensorMode*    pSensorSelectMode,
        SensorSelectBestResolution* pPickedResolution);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsMatchingMaxResolution
    ///
    /// @brief  Method to IsMatching max resolution
    ///
    /// @param  index           mode index in the binary
    /// @param  pModeInfo       Pointer to mode information in sensor driver
    /// @param  pMpix           Pointer to select best sensor resolution
    ///
    /// @return True if max resolution condition matches
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL IsMatchingMaxResolution(
        const ChiSensorModeInfo*    pModeInfo,
        SensorSelectBestResolution* pPickedResolution);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsMatchingHFR
    ///
    /// @brief  Method to IsMatching HFR mode
    ///
    /// @param  index               Mode index in the binary
    /// @param  pModeInfo           Pointer to mode information in sensor driver
    /// @param  pSensorSelectMode   Pointer to SensorSelectMode as requested by application
    ///
    /// @return True if the mode is HFR
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL IsMatchingHFR(
        const ChiSensorModeInfo* pModeInfo,
        const DesiredSensorMode* pSensorSelectMode);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsMatchingDefaultMode
    ///
    /// @brief  Method to IsMatching DEF mode
    ///
    /// @param  index           mode index in the binary
    /// @param  pModeInfo       Pointer to mode information in sensor driver
    ///
    /// @return True if the mode is Default
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL IsMatchingDefaultMode(
        const ChiSensorModeInfo* pModeInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsMatchingRHDRMode
    ///
    /// @brief  Method to IsMatching ZZHDR mode
    ///
    /// @param  index           mode index in the binary
    /// @param  pModeInfo       Pointer to mode information in sensor driver
    ///
    /// @return True if the mode is ZZHDR
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static BOOL IsMatchingRHDRMode(
        const ChiSensorModeInfo* pModeInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsMatchingIHDRMode
    ///
    /// @brief  Method to IsMatching IHDR mode
    ///
    /// @param  index           mode index in the binary
    /// @param  pModeInfo       Pointer to mode information in sensor driver
    ///
    /// @return True if the mode is IHDR
    ///////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////
    static BOOL IsMatchingIHDRMode(
        const ChiSensorModeInfo* pModeInfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SelectUseCase
    ///
    /// @brief  Select usecase
    ///
    /// @param  cameraId          Camera Id
    /// @param  pSensorSelectMode Pointer to ChxSensorSelectMode
    ///
    /// @return True if aspect ratio condition matches else false
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static SensorModeSelectUseCase SelectUseCase(
        UINT32                   cameraId,
        const DesiredSensorMode* pSelectModeInfo);
};

#endif // CHXSENSORSELECTMODE_H
