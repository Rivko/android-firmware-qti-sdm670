////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxstatsdebugdatatypes.h
/// @brief Debug data types class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXSTATSDEBUGDATATYPES_H
#define CAMXSTATSDEBUGDATATYPES_H

#include "camxosutils.h"

// Divide the 2-byte TypeID space into sectors, each allowing 256 types.
// This is merely a guideline; developers need not strictly adhere to this size.
#define TAGTYPE_SECTOR_SIZE         0x100
#define TAGTYPE_BASIC_MAX           0x40
#define TAGTYPE_AEC_MAX             (TAGTYPE_BASIC_MAX + (TAGTYPE_SECTOR_SIZE * 1))
#define TAGTYPE_AWB_MAX             (TAGTYPE_BASIC_MAX + (TAGTYPE_SECTOR_SIZE * 2))
#define TAGTYPE_AF_MAX              (TAGTYPE_BASIC_MAX + (TAGTYPE_SECTOR_SIZE * 3))
#define TAGTYPE_ASD_MAX             (TAGTYPE_BASIC_MAX + (TAGTYPE_SECTOR_SIZE * 4))
#define TAGTYPE_AFD_MAX             (TAGTYPE_BASIC_MAX + (TAGTYPE_SECTOR_SIZE * 5))
#define TAGTYPE_TUNING_MAX          (TAGTYPE_BASIC_MAX + (TAGTYPE_SECTOR_SIZE * 6))
#define TAGTYPE_RESERVED_MAX        (TAGTYPE_BASIC_MAX + (TAGTYPE_SECTOR_SIZE * 7))
#define TAGTYPE_OEM_START           (TAGTYPE_SECTOR_SIZE * 255)
#define TAGTYPE_OEM_MAX             (TAGTYPE_BASIC_MAX + (TAGTYPE_SECTOR_SIZE * 255)) // This is the limit for 2-bytes


// Divide the 2-byte TagID space into sectors, each allowing 512 tags.
// This should allow for a maximum of 128 sectors. Note that there is no requirement to strictly adhere to this guideline; so
// one could have TagID spaces much smaller or larger than this size defined here.
// Disclaimer: existing size definitions determine the start of the subsequent TagID space - and as such anything that is
// already released cannot be modified. Below sizes can therefore not change without adding necessary placeholders to ensure
// enum values stay constant across releases.
#define TAGID_SECTOR_SIZE           0x200
#define TAGID_BUILTIN_MAX           0x40
#define TAGID_AEC_MAX               (TAGID_BUILTIN_MAX + (TAGID_SECTOR_SIZE * 1))
#define TAGID_AWB_MAX               (TAGID_BUILTIN_MAX + (TAGID_SECTOR_SIZE * 2))
#define TAGID_AF_MAX                (TAGID_BUILTIN_MAX + (TAGID_SECTOR_SIZE * 3))
#define TAGID_ASD_MAX               (TAGID_BUILTIN_MAX + (TAGID_SECTOR_SIZE * 4))
#define TAGID_AFD_MAX               (TAGID_BUILTIN_MAX + (TAGID_SECTOR_SIZE * 5))
#define TAGID_TUNING_MAX            (TAGID_BUILTIN_MAX + (TAGID_SECTOR_SIZE * 6))
#define TAGID_OEM_START             (TAGID_SECTOR_SIZE * 127)
#define TAGID_OEM_MAX               (TAGID_BUILTIN_MAX + (TAGID_SECTOR_SIZE * 127))

CAMX_BEGIN_PACKED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// NOTE: It must be enforced that new tags only get added to the bottom of this enum of each reserved section.
/// Aka add the new definition right above the "XXX  tag IDs - END". ID values which already exist must not change.
/// @todo (CAMX-1234): tag values should be hard coded and guaranteed to never change
//      so eliminate user error by hard coding values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///< @brief Defines the type of IDs for the debug data tag
///  NOTE: The values defined in this enum MUST NOT change! If an ID is being deprecated - developers can rename the ID to
///        to indicate that it is deprecated - but should not remove it.
enum class DebugDataTagID : UINT16
{
    UnusedSpace                         = 0,                            ///< Unused space in debug data buffer
    ReservedBasic                       = TAGID_BUILTIN_MAX,            ///< Reserved above can only be used as basic type definition

    // AEC tag IDs - START
    AECConfiguration,
    AECFrameHistory,
    AECModeHistory,
    AECMetering,
    AECConvergence,
    AECArbitration,
    AECExtension,
    AECStatsLumaRegion,
    AECExtremeColor,
    AECHistogram,
    AECLEDTable,
    AECFlash,
    AECTagIDMax                         = TAGID_AEC_MAX,
    // AEC tag IDs - END

    // AWB tag IDs - START
    AWBSetParam,                        ///< Deprecated
    AWBReferencePoints,
    AWBMWBGains,
    AWBCurFrameDecision,
    AWBPreFrameDecision,
    AWBPreValidDAPoint,
    AWBPreValidGA,
    AWBFrameIndex,
    AWBLuxIndex,
    AWBSubSampleData,
    AWBStatsScreenData,
    AWBIlluWeightVector,
    AWBDistWeightVector,
    AWBCustRefPoint,
    AWBMLCZData,
    AWBAGWData,
    AWBSAGen1InstanceCnt,
    AWBSAGen1Data,
    AWBSAFaceAssist,
    AWBDecisionAggregator,
    AWBTrigleGainAdjust,
    AWBTemporalConverg,
    AWBDSFData,
    AWBSensorCallibration,
    AWBHistoryInfo,
    AWBFlashData,
    AWBStatsInfo,
    AWBBestShotData,
    AWBLockAWBPoint,
    AWBLCDFlashData,
    AWBLEDTorchData,
    AWBBGStats,
    AWBSADualcamSync,
    AWBTagIDMax                         = TAGID_AWB_MAX,
    // AWB tag IDs - END

    // AF tag IDs - START
    AFFarEnd,
    AFNearEnd,
    AFFocalLength,
    AFFocalNumber,
    AFPixelSize,
    AFFocusDistance,
    AFActuatorSensitivity,
    AFCamifWidth,
    AFCamifHeight,
    AFActuator,
    AFConfigFrameId,
    AFGammaTableEnable,
    AFGammaTableLength,
    AFGammaTableContent,
    AFHorizontalKernel,
    AFVerticalKernel,
    AFSoftwareKernel,
    AFRoiInfo,
    AFRoiPattern,
    AFRoiGridSizeH,
    AFRoiGridSizeV,
    AFRoiGridGapH,
    AFRoiGridGapV,
    AFInputSel,
    AFGreenChannelSel,
    AFYChannelWeight,
    AFScaleEnable,
    AFScaleM,
    AFScaleN,
    AFCurrentMonitorIndex,
    AFMonitorHistorySize,
    AFActiveMonMask,
    AFMonitorCommonData,
    AFMonitorCAF,
    AFMonitorPDAF,
    AFMonitorTOF,
    AFCurrentConvergeIndex,
    AFConvergeHistorySize,
    AFActiveAlgoMask,
    AFConvergeCommonData,
    AFConvergeSingleAFData,
    AFConvergeCafFineScanData,
    AFConvergeTofData,
    AFConvergePDData,
    AFConvergeFollowerData,
    AFTagIDMax                          = TAGID_AF_MAX,
    // AF tag IDs - END

    // ASD tag IDs - START
    ASDTagIDMax                         = TAGID_ASD_MAX,
    // ASD tag IDs - END

    // AFD tag IDs - START
    AFDTagIDMax                         = TAGID_AFD_MAX,
    // AFD tag IDs - END

    // Tuning tag IDs - START
    TuningAECData,
    TuningAWBData,
    TuningSensorPixelFormat,
    TuningSensorNumPixelsPerLine,
    TuningSensorNumLinesPerFrame,
    TuningSensorMaxLineCount,
    TuningSensorResoultion,
    TuningSensorCropInfo,
    TuningBinningTypeH,
    TuningBinningTypeV,
    TuningIFEGammaPackedLUT,
    TuningIFEGTMPackedLUT,
    TuningIFELSCPackedMesh,
    TuningBPSPedestalRegister,
    TuningBPSPedestalPackedLUT,
    TuningBPSLinearizationRegister,
    TuningBPSLinearizationPackedLUT,
    TuningBPSBPCPDPCRegister,
    TuningBPSBPCPDPCPackedLUT,
    TuningBPSHDRRegister,
    TuningBPSGICRegister,
    TuningBPSGICPackedNoiseLUT,
    TuningBPSABFRegister,
    TuningBPSABFPackedLUT,
    TuningBPSLSCRegister,
    TuningBPSLSCPackedMesh,
    TuningBPSWBRegister,
    TuningBPSDemosaicRegister,
    TuningBPSDemuxRegister,
    TuningBPSCCRegister,
    TuningBPSGTMPackedLUT,
    TuningBPSGammaLUT,
    TuningBPSCSTRegister,
    TuningBPSHNRRegister,
    TuningBPSHNRPackedLUT,
    TuningIPEGammaPackedLUT,
    TuningBPSModulesConfigRegister,
    TuningBPSTriggerModulesData,
    TuningBPSensorConfigData,
    TuningIPEGammaPackedLUTOffline,
    TuningIPEModulesConfigRegister,
    TuningIPEModulesConfigRegisterOffline,
    TuningIPEICEInputPackedLUT,
    TuningIPEICEInputPackedLUTOffline,
    TuningIPEICEReferencePackedLUT,
    TuningIPEICEReferencePackedLUTOffline,
    TuningIPEANRRegister,
    TuningIPEANRRegisterOffline,
    TuningIPETFRegister,
    TuningIPETFRegisterOffline,
    TuningIPECACRegister,
    TuningIPECACRegisterOffline,
    TuningIPECSTRegister,
    TuningIPECSTRegisterOffline,
    TuningIPELTMPackedLUT,
    TuningIPELTMPackedLUTOffline,
    TuningIPECCRegister,
    TuningIPECCRegisterOffline,
    TuningIPE2DLUTRegister,
    TuningIPE2DLUTRegisterOffline,
    TuningIPE2DLUTPackedLUT,
    TuningIPE2DLUTPackedLUTOffline,
    TuningIPEChromaEnhancementRegister,
    TuningIPEChromaEnhancementRegisterOffline,
    TuningIPEChromasuppressionRegister,
    TuningIPEChromasuppressionRegisterOffline,
    TuningIPESCERegister,
    TuningIPESCERegisterOffline,
    TuningIPEASFRegister,
    TuningIPEASFRegisterOffline,
    TuningIPEASFPackedLUT,
    TuningIPEASFPackedLUTOffline,
    TuningIPEUpscalerPackedLUT,
    TuningIPEUpscalerPackedLUTOffline,
    TuningIPEGrainAdderPackedLUT,
    TuningIPEGrainAdderPackedLUTOffline,
    TuningBPSHNRFace,
    TuningIPEASFFace,
    TuningIPEASFFaceOffline,
    TuningIPEANRFace,
    TuningIPEANRFaceOffline,
    TuningIPELTMExposureIndex,
    TuningIPELTMExposureIndexOffline,
    TuningBPSExposureGainRatio,
    TuningBPSNodeInfo,
    TuningIPENodeInfo,
    TuningIPENodeInfoOffline,
    TuningIPETuningMode,
    TuningIPETuningModeOffline,
    TuningTagIDMax                      = TAGID_TUNING_MAX,
    // Tuning tag IDs - END
    TuningOEMStart                      = TAGID_OEM_START,
    TuningBPSOEMTuningData              = TuningOEMStart,
    TuningOEMTagIDMax                   = TAGID_OEM_MAX,
} CAMX_PACKED;

///< @brief Defines the type of the debug data tag
///  NOTE: The values defined in this enum MUST NOT change! If an ID is being deprecated - developers can rename the ID to
///        to indicate that it is deprecated - but should not remove it.
enum class DebugDataTagType : UINT16
{
    TypeDefinition                      = 1,                            ///< Payload: DebugDataTag + list of TypedefFieldInfo
    Char,
    Bool,
    UInt8,
    UInt16,
    UInt32,
    UInt64,
    UQNumber,                                                           ///< Payload: DebugDataUQNumber
    URational,                                                          ///< Payload: DebugDataURational
    Int8,
    Int16,
    Int32,
    Int64,
    QNumber,                                                            ///< Payload: DebugDataQNumber
    Rational,                                                           ///< Payload: DebugDataRational
    Float,
    Double,
    TwoDimArray,                                                        ///< Payload: DebugData2DArrayHeader + array data
    EndBasicTypes                       = TAGTYPE_BASIC_MAX,            ///< Reserve up to this number for basic types

    // Custom (non-basic) types - START
    CustomStart                         = EndBasicTypes,                ///< custom (non-basic) type
    // AEC Custom (non-basic) types - START
    AECTypeBegin,
    AECROIDim                           = AECTypeBegin,
    AECConfiguration,
    AECFrameHistory,
    AECModeHistory,
    AECMetering,
    AECConvergence,
    AECArbitration,
    AECExtension,
    AECStatsLumaRegion,
    AECExtremeColor,
    AECHistogram,
    AECLEDSetting,
    AECLEDTable,
    AECFlash,
    AECTypeEnd                          = AECFlash,
    AECTypeMax                          = TAGTYPE_AEC_MAX,
    // AEC Custom (non-basic) types - END

    // AWB Custom (non-basic) types - START
    AWBTypeBegin,
    AWBPoint                            = AWBTypeBegin,
    AWBZone,
    AWBZonePData,
    AWBDecisionPData,
    AWBModulePData,
    AWBGains,
    AWBTriggerPoint,
    AWBTriggerFloatData,
    AWBStatsRectangle,
    AWBTriggerNonLastLevelPData,
    AWBcustRefPt,
    AWBSATargetPData,
    AWBSAConfidencePData,
    AWBSFSubSmpPData,
    AWBSFStatsScrPData,
    AWBSFIlluWVPData,
    AWBSFDistWVPData,
    AWBSFCustRefPtPData,
    AWBSFMLCZPData,
    AWBSAAGWPData,
    AWBDASAData,
    AWBFaceAssistSAPData,
    AWBDAPData,
    AWBTriglGAPData,
    AWBTmpConvPData,
    AWBDSFPData,
    AWBSensorCalPData,
    AWBSAGen1PData,
    AWBHistPData,
    AWBFlashPData,
    AWBStatInfo,
    AWBStatPData,
    AWBBestShotPData,
    AWBSpecIlluPData,
    AWBLCDFlashPData,
    AWBLEDTorchPData,
    AWBStatsSensorInfo,                 ///< Deprecated
    AWBStatsWindowInfo,                 ///< Deprecated
    AWBIlluminatFactor,                 ///< Deprecated
    AWBIlluminantsCalibrationFactor,    ///< Deprecated
    AWBGeometricalDisparityCalibration, ///< Deprecated
    AWBStatsLEDCalibrationDataInput,    ///< Deprecated
    AWBManualConfiguration,             ///< Deprecated
    AWBStatsFaceData,                   ///< Deprecated
    AWBStatsFaceHistogram,              ///< Deprecated
    AWBStatsFaceInformation,            ///< Deprecated
    AWBProcessTrigGain,
    AWBFlashInformation,                ///< Deprecated
    AWBSetParamInfo,                    ///< Deprecated
    AWBSFDistWVPTrigger2D,
    AWBBGChannelData,
    AWBBGStatsData,
    AWBSADualcamSyncPData,
    AWBTypeEnd                          = AWBSADualcamSyncPData,
    AWBTypeMax                          = TAGTYPE_AWB_MAX,
    // AWB Custom (non-basic) types - END

    // AF Custom (non-basic) types - START
    AFTypeBegin,
    AFROIInfo                           = AFTypeBegin,
    AFCommonHistory,
    AFConvergeCommon,
    AFSingleAFHistory,
    AFSingleAFData,
    AFCAFFineScanHistory,
    AFCAFFineScanData,
    AFTOFHistory,
    AFTimeOfFlight,
    AFPDData,
    AFPDHistory,
    AFPhaseDifference,
    AFFollower,
    AFMonitorCommon,
    AFValueMonitorData,
    AFMonitorCAF,
    AFMonitorPDAF,
    AFMonitorTOF,
    AFGammaTable,
    AFFocusROI,
    AFFIRFilter,
    AFIIRFilter,
    AFCoringInfo,
    AFFilterKernel,
    AFSWKernel,
    AFTypeEnd                           = AFSWKernel,
    AFTypeMax                           = TAGTYPE_AF_MAX,
    // AF Custom (non-basic) types - END

    // ASD Custom (non-basic) types - START
    ASDTypeBegin,
    ASDTypeEnd,
    ASDTypeMax                          = TAGTYPE_ASD_MAX,
    // ASD Custom (non-basic) types - END

    // AFD Custom (non-basic) types - START
    AFDTypeBegin,
    AFDTypeEnd,
    AFDTypeMax                          = TAGTYPE_AFD_MAX,
    // AFD Custom (non-basic) types - END

    // Tuning metadata custom - START
    TuningTypeBegin,
    TuningAECExposureData               = TuningTypeBegin,
    TuningAECFrameControl,
    TuningAWBFrameControl,
    TuningSensorResolution,
    TuningSensorCropInfo,
    TuningLSCMeshLUT,
    TuningGammaCurve,
    TuningGTMLUT,
    TuningPedestalLUT,
    TuningBPSBPCPDPCConfig,
    TuningBPSHDRConfig,
    TuningABFLUT,
    TuningBPSHNRLUT,
    TuningBPSTriggerData,
    TuningBPSSensorConfig,
    TuningICELUT,
    TuningANRConfig,
    TuningTFConfig,
    TuningLTMLUT,
    Tuning2DLUTLUT,
    TuningASFLUT,
    TuningUpscalerLUT,
    TuningGrainAdderLUT,
    TuningFaceData,
    TuningGammaIPECurve,
    TuningIQNodeInfo,
    TuningModeInfo,
    TuningTypeEnd                       = TuningModeInfo,
    TuningTypeMax                       = TAGTYPE_TUNING_MAX,
    // Tuning metadata custom - END

    // Reserved EOM (non-basic) types - START
    ReservedTypeMax                     = TAGTYPE_RESERVED_MAX,
    OEMBegin                            = TAGTYPE_OEM_START,
    // Reserved OEM (non-basic) types - END
    OEMEnd                              = TAGTYPE_OEM_MAX,
    // All metadata custom - END
    CustomEnd                           = TAGTYPE_OEM_MAX

} CAMX_PACKED;


///< @brief List of deprecated debug data tag types as defined in DebugDataTagType enum.
///         All deprecated tags must be listed here but remain in the DebugDataTagType enum to keep the order of existing tags
static DebugDataTagType DebugDataTagTypeDeprecated[] =
{
    DebugDataTagType::AWBStatsSensorInfo,
    DebugDataTagType::AWBStatsWindowInfo,
    DebugDataTagType::AWBIlluminatFactor,
    DebugDataTagType::AWBIlluminantsCalibrationFactor,
    DebugDataTagType::AWBGeometricalDisparityCalibration,
    DebugDataTagType::AWBStatsLEDCalibrationDataInput,
    DebugDataTagType::AWBManualConfiguration,
    DebugDataTagType::AWBStatsFaceData,
    DebugDataTagType::AWBStatsFaceHistogram,
    DebugDataTagType::AWBStatsFaceInformation,
    DebugDataTagType::AWBFlashInformation,
    DebugDataTagType::AWBSetParamInfo,
};

///< @brief List of deprecated debug data tag IDs as defined in DebugDataTagID enum.
///         All deprecated tags must be listed here but remain in the DebugDataTagID enum to keep the order of existing tags
static DebugDataTagID DebugDataTagIDDeprecated[] =
{
    DebugDataTagID::AWBSetParam
};

typedef UINT32  TagCount;           ///< Data type representing the Debug Data tag count
typedef UINT16  TypedefFieldLen;    ///< Data type representing the Debug Data type-definition field length
typedef UINT8   TypedefFieldID;     ///< Data type representing the Debug Data type-definition field ID

///< @brief Encapsulates a debug data tag header without any payloads
struct DebugDataTag
{
    /// Unique ID that identifies this tag
    DebugDataTagID    id;
    /// Type of the data associated with this tag
    DebugDataTagType  type;
    /// Number of times the data appears (more than 1 indicates array,
    /// while 0 indicates no data at all). Size in bytes of data is
    /// determined by sizeof(tagType) * Count
    ///
    /// Special case: when the type is DebugDataTagType::Typedef, the count determines
    /// the number of fields
    TagCount          count;
} CAMX_PACKED;

///< @brief Represents a single field within a type-definition tag
struct TypedefFieldInfo
{
    TypedefFieldID      fieldID;    ///< The field ID. Thsi should start at 1 for each type-definition
    DebugDataTagType    type;       ///< The data type of this field
    TypedefFieldLen     count;      ///< Number of occurances of this field. Set to 1 if a single field, otherwise set to
                                    ///  number of elements for an array field
} CAMX_PACKED;

///< @brief Debug data payload for DebugDataTagType::QNumber to represent a signed Q number
///         Float representation can be obtained by evaluating    value * pow(2, -1*qNumber)
struct DebugDataQNumber
{
    UINT8   qNumber;    ///< Exponent
    INT32   value;      ///< Value
} CAMX_PACKED;

///< @brief Debug data payload for DebugDataTagType::UQNumber to represent an unsigned Q number
///         Float representation can be obtained by evaluating    value * pow(2, -1*qNumber)
struct DebugDataUQNumber
{
    UINT8   qNumber;    ///< Exponent
    UINT32  value;      ///< Value
} CAMX_PACKED;

///< @brief Debug data payload for DebugDataTagType::Rational to represent a signed rational number
struct DebugDataRational
{
    INT32 numerator;    ///< Rational number's numerator
    INT32 denominator;  ///< Rational number's denominator
} CAMX_PACKED;

///< @brief Debug data payload for DebugDataTagType::URational to represent an unsigned rational number
struct DebugDataURational
{
    UINT32 numerator;   ///< Rational number's numerator
    UINT32 denominator; ///< Rational number's denominator
} CAMX_PACKED;

///< @brief Debug data header for 2D arrays to describe the array dimentions
struct DebugData2DArrayHeader
{
    DebugDataTagType    underlyingType; ///< Type of each element of the 2D array
    UINT16              dimension1;     ///< Array dimension 1 - or number of rows
    UINT16              dimension2;     ///< Array dimension 2 - or number of columns
} CAMX_PACKED;

CAMX_END_PACKED

#endif // CAMXSTATSDEBUGDATATYPES_H
