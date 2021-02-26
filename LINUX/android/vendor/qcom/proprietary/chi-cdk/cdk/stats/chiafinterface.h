////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chiafinterface.h
/// @brief Auto Focus Algorithm interface definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NOWHINE FILE GR016: Typedefs must not be used for structs, classes, and enumerations

#ifndef CHIAFINTERFACE_H
#define CHIAFINTERFACE_H

#include "chistatsinterfacedefs.h"
#include "chiafcommon.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// @brief Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

/// @brief Identifies the type of input
typedef enum
{
    AFInputTypeInvalid       = -1,          ///< Invalid type
    AFInputTypeBAFStats,                    ///< Bayer focus statistics
                                            ///  Payload: AFBAFStatistics
    AFInputTypeBAFStatsLEF,                 ///< Bayer focus statistics for Long Exposure
                                            ///  Payload: AFBAFStatistics
    AFInputTypeBGStats,                     ///< Bayer grid statistics
                                            ///  Payload: AFBGStatistics
    AFInputTypeVendorTag,                   ///< Vendor tag data
    AFInputTypeRequestId,                   ///< Request number type.
                                            ///  Payload type: UINT64
                                            ///  Payload type: StatsVendorTag
    AFInputTypeDebugData,                   ///< Debug data information. This is optional and can be NULL
                                            ///  Payload type: StatsDataPointer
    AFInputTypeStatsChiHandle,              ///< Algo needs to use this handle when querying vendor tags
                                            ///  Payload type: CHISTATSHANDLE
    AFInputTypeCameraInfo,                  ///< Set camera information regarding algo role(master/slave) and camera ID.
                                            ///  Payload type: StatsCameraInfo
    AFInputTypeLock,                        ///< Set AF Lock
                                            ///  Payload type: BOOL
    AFInputTypeLastIndex,                   ///< Last valid index of the enum
                                            ///  Payload type: None
    AFInputTypeMax           = 0x7FFFFFFF   ///< Max Type

} AFAlgoInputType;

/// @brief Represents AF algorithm input parameter
typedef struct
{
    VOID*           pAFInput;       ///< Pointer to AF input param data. See AFAlgoInputType for details
    UINT32          sizeOfInput;    ///< Size of data pointed to by pAFInput
    AFAlgoInputType inputType;      ///< Type of the input parameter
} AFAlgoInput;

/// @brief Represents AF input
typedef struct
{
    AFAlgoInput*    pAFInputs;          ///< Pointer to AF input - See AFAlgoInputType for details
    SIZE_T          numberOfInputParam; ///< Number of input in pAFInput
} AFAlgoInputList;

/// @brief Represents AF output type
typedef enum
{
    AFOutputTypeInvalid                     = -1,           ///< Invalid type
    AFOutputTypeStatusParam,                                ///< AF algorithm status
                                                            ///  Payload : AFAlgoStatusParam
    AFOutputTypeBAFFloatingWindowConfig,                    ///< BAF floating window config
                                                            ///  Payload: AFBAFFloatingWindowConfig
    AFOutputTypeBAFFloatingWindowROIConfig,                 ///< BAF floating window ROI config
                                                            ///  Payload: AFBAFFloatingWindowROIConfig
    AFOutputTypeROIDim,                                     ///< ROI rectangle coordinates
                                                            ///  Payload: AFROIDimension
    AFOutputTypePDAFConfig,                                 ///< PDAF window configuration
                                                            ///  Payload: AFAlgoPDAFWindowConfig
    AFOutputTypeMoveLensParam,                              ///< Move lens configuration
                                                            ///  Payload: AFLensPositionInfo
    AFOutputTypeFOVCParam,                                  ///< FOVC configuration
                                                            ///  Payload: AFFOVCInfo
    AFOutputTypeSoftwareStatsConfig,                        ///< Software Stats Configuration
                                                            ///  Payload: AFSoftwareFilterConfig
    AFOutputTypeExposureCompensation,                       ///< Exposure Compensation Enable
                                                            ///  Payload: AFAlgoExposureCompensation
    AFOutputTypeFocusMode,                                  ///< Focus Mode
                                                            ///  Payload: AFAlgoFocusMode
    AFOutputTypeFocusValue,                                 ///< Focus Value
                                                            ///  Payload: FLOAT
    AFOutputTypeDebugData,                                  ///< Debug Data
                                                            ///  Payload: StatsDataPointer
    AFOutputTypeSpotLightDetected,                          ///< Spot Light Detected
                                                            ///  Payload: AFAlgoSpotLightInfo
    AFOutputTypeVendorTag,                                  ///< Vendor Tag Data
                                                            ///  Vendor tag data. This is the list of vendor tags AF algorithm outputs
    AFOutputPublishingVendorTagsInfo,                       ///< The vendor tags published by the algorithm
                                                            ///  Payload in: NULL. Payload out: StatsVendorTagList
    AFOutputTypeCropMagnificationFactor,                    ///< Magnification Factor Output
                                                            ///  Payload: AFAlgoCropMagnificationFactor
    AFOutputTypeLastIndex,                                  ///< Last index of the enum
                                                            ///  Payload type: None
    AFOutputTypeMax                         = 0x7FFFFFFF    ///< Max Type
} AFAlgoOutputType;

/// @brief Represents AF individual output from the algorithm
typedef struct
{
    VOID*               pAFOutput;              ///< Pointer to the payload. See AFAlgoOutputType for details
    UINT32              sizeOfAFOutput;         ///< Size of input payload pointed to by pAFOutput
    UINT32              sizeOfWrittenAFOutput;  ///< Size of payload written back
    AFAlgoOutputType    outputType;             ///< Type of the payload
} AFAlgoOutput;

/// @brief Represents AF output from the algorithm
typedef struct
{
    AFAlgoOutput*       pAFOutputs;     ///< Pointer to AFAlgoOutputs array
    UINT32              outputCount;    ///< Number of elements in pIndividualAlgorithmOutput
} AFAlgoOutputList;

/// @brief Represents the focus distances for near/far/optimal
///< distance for this sensor
typedef struct
{
    FLOAT nearFocusDistance;    ///< Near Focus Distance
    FLOAT optimalFocusDistance; ///< Optimal Focus Distance
    FLOAT farFocusDistance;     ///< Far Focus Distance
} AFFocusDistance;

/// @brief Indicates the type of AF GetParameter query
typedef enum
{
    AFGetParamTypeInvalid          = -1,        ///< Invalid
    AFGetParamTypeDebugData,                    ///< Queries the AF debug data.
                                                ///  Payload.in NULL. Payload out StatsDataPointer
    AFGetParamTypeSWVersion,                    ///< Queries the SW version
                                                ///  Payload out: StatsAlgorithmVersion
    AFGetParamFocusDistances,                   ///< Focus Distances from AF Algorithm
    AFGetParamCurrentLensPosition,              ///< Current Lens Position
                                                ///  Payload in NULL Payload Out: UINT32
    AFGetParamFocusStatus,                      ///< Current Focus Status
                                                ///  Payload in NULL Payload Out: AFFocusStatus
    AFGetParamFocusMode,                        ///< Current Focus Mode
                                                ///  Payload in NULL Payload Out: AFFocusMode
    AFGetParamBAFFloatingWindowConfig,          ///< BAF floating window config
                                                ///  Payload: AFBAFFloatingWindowConfig
    AFGetParamBAFFloatingWindowROIConfig,       ///< BAF floating window ROI config
                                                ///  Payload: AFBAFFloatingWindowROIConfig
    AFGetParamROIDim,                           ///< ROI rectangle coordinates
    AFGetParamVendorTagList,                    ///< Vendor tag data. This is the list of vendor tags AF algorithm outputs
                                                ///  payload: StatsVendorTagInfoList
    AFGetParamDependentVendorTags,              ///< The vendor tags dependent by the algorithm
                                                ///  Payload in: NULL. Payload out: StatsVendorTagInfoList
    AFGetParamPDAFWindowConfig,                 ///< PDAF fixed window config
                                                ///  Payload: PDLibWindowConfig
    AFGetParamPassCameraInfo,                   ///< The current camera infomation pass to singleton algorithm
                                                ///  Payload in: StatsCameraInfo. Payload out: NULL
    AFGetParamInfoForPeer,                      ///< Payload: Camera info require for peer
    AFGetParamLastIndex,                        ///< Last index of the enum
    AFGetParamTypeMax              = 0x7FFFFFFF ///< Max GetParam type
} AFAlgoGetParamType;

/// @brief Represents AF input data and type of data
///< being requested using getParam
typedef struct
{
    VOID*               pInputData;         ///< Pointer to input data given in the query information to generate the output
    UINT32              sizeOfInputData;    ///< Size of input data pointed to by pInputData
    AFAlgoGetParamType  type;               ///< Type of the meta-data query
} AFAlgoGetParamInput;

/// @brief Represents AF module inputs to the AF algorithm
typedef struct
{
    AFAlgoGetParamInput*    pGetParamInputs;    ///< Pointer to AFAlgoGetParamInput array
    UINT32                  getParamInputCount; ///< Number of inputs
} AFAlgoGetParamInputList;


/// @brief Represents AF get output information from the algorithm
typedef struct
{
    VOID*                       pGetParamOutput;                ///< Pointer to the payload. See AFAlgoGetParamType for details
                                                                ///  Algorithm implementer needs to do a deep copy to the given memory
    UINT32                      sizeOfGetParamOutput;           ///< Size of input payload pointed to by pGetParamOutput
    UINT32                      sizeOfWrittenGetParamOutput;    ///< Size of payload written back. If no output set to zero.
    AFAlgoGetParamType          getParamOutputType;             ///< Type of the payload
} AFAlgoGetParamOutput;

/// @brief Represents AF output data returned by AFGetParam()
typedef struct
{
    AFAlgoGetParamOutput*   pGetParamOutputs;       ///< Pointer to AFAlgorithmOutput array.
    UINT32                  getParamOutputCount;    ///< Number of elements in pAFOutputs
} AFAlgoGetParamOutputList;

/// @brief Represents the input param given to the AFGetParam,
///< and contains any associated data necessary to support that query
typedef struct
{
    AFAlgoGetParamInputList     inputInfo;    ///< Information describing the query
    AFAlgoGetParamOutputList    outputInfo;   ///< Placeholder for the queried data
} AFAlgoGetParam;

/// @brief Identifies the type of input parameter
typedef enum
{
    AFSetParamTypeInvalid                   = -1,           ///< Invalid type
    AFSetParamTypeHardwareCapabilities,                     ///< Sets Hardware Capabilities
                                                            /// Payload AFHardwareCapability
    AFSetParamTypeTuningData,                               ///< Tuning data type
                                                            ///  Payload type: StatsTuningData
    AFSetParamTypeISPStreamInfo,                            ///< Sets the stream information from ISP
                                                            ///  Payload  StatsISPStreamInfo
    AFSetParamTypeSensorInfo,                               ///< Sensor information
                                                            ///  Payload: AFSensorInfo
    AFSetParamTypeAECInfo,                                  ///< AEC output information
                                                            ///  Payload: AFAECInfo
    AFSetParamTypeROI,                                      ///< ROI information in case of Touch/Face focus mode
                                                            ///  Payload: AFROIInfo
    AFSetParamTypeGravityData,                              ///< gravity vector information
                                                            ///  Payload: AFGravityData
    AFSetParamTypeGyroData,                                 ///< gyro information
                                                            ///  Payload: AFGyroData
    AFSetParamTypeLinearAccelerometerData,                  ///< LinearAccelerometer data
                                                            ///  Payload: AFLinearAccelerationData
    AFSetParamTypePDAFData,                                 ///< PDAF input to AF algorithm
                                                            ///  Payload: AFPDAFData
    AFSetParamTypeTOFData,                                  ///< Laser Data input to AF algorithm
                                                            ///  Payload: AFTOFData
    AFSetParamTypeDebugData,                                ///< Buffer to collect debug data
                                                            ///  Payload: StatsDataPointer
    AFSetParamTypeMeteringMode,                             ///< Indicates the chosen auto focus metering mode
                                                            ///  Payload:AFMeteringType
    AFSetParamTypeFocusMode,                                ///< Indicates the chosen Focus Mode
                                                            ///  Payload: AFFocusMode
    AFSetParamTypeFocusControlCommand,                      ///< Indicates focus command from upper layer
                                                            ///  Payload:AFControlCommand
    AFSetParamTypeRunMode,                                  ///< Indicates the camera mode
                                                            ///  Payload: AFRunMode
    AFSetParamTypeManualLensMoveInfo,                       ///< Indicates Manual Lens Move Info
                                                            ///  Payload : AFManualLensMoveInfo
    AFSetParamTypeSoftwareStatistics,                       ///< Preview assisted auto focus statistics
                                                            ///  Payload: AFSoftwareStatistics
    AFSetParamTypeBGStats,                                  ///  Payload: Bayer grid stats
                                                            ///  Payload: AFSetParamTypeBGStats
    AFSetParamTypeFaceROI,                                  ///< Face ROIs
                                                            ///  Payload: AFFaceROIInfo
    AFSetParamTypeCameraInfo,                               ///< Sets camera infomation
                                                            ///  Payload StatsCameraInfo
    AFSetParamTypeSettings,                                 ///< Sets AF Algo Settings
                                                            ///  Payload: AFAlgoSettingInfo
    AFSetParamPeerInfo,                                     ///< Payload Is VOID* have the information of
                                                            ///  Peer Camera 
    AFSetParamCropWindow,                                   ///< Sets crop information to algorithm
                                                            ///  Payload: StatsRectangle
    AFSetParamLastIndex,                                    ///< Last valid index of the enum
                                                            ///  Payload: None
    AFSetParamTypeMax                       = 0x7FFFFFFF    ///< Max Type
} AFAlgoSetParamType;

/// @brief Represents an AF core input parameter
typedef struct
{
    VOID*                  pAFSetParam;     ///< Pointer to AF input param data. See AFAlgoSetParamType for details
    SIZE_T                sizeOfSetParam;   ///< Size of data pointed to by pAFSetParam
    AFAlgoSetParamType    setParamType;     ///< Type of the input parameter
} AFAlgoSetParam;

/// @brief Represents AF input parameters
typedef struct
{
    AFAlgoSetParam*    pAFSetParams;        ///< Pointer to AF input param. See AFAlgoSetParamType for details
    SIZE_T              numberOfSetParam;   ///< Number of input parameters
} AFAlgoSetParamList;

/// @brief Represents the enumerated type to get indicate
/// supported Algorithm features
typedef enum
{
    AFAlgoFeatureNoFeature  = 0,            ///< No features supported
    AFAlgoFeatureMax        = 0x7FFFFFFF    ///< All features supported
} AFAlgoFeatureType;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Interface for AF algorithm.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct CHIAFAlgorithm
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AFProcess
    ///
    /// @brief  Runs the stats algorithm on the given inputs and input param set by AFSetParam()
    ///
    /// @param  pAFAlgo         Pointer to CHIAFAlgorithm instance
    /// @param  pInput          Pointer to stats inputs interface. These are the inputs
    ///                         that changes per process
    /// @param  pOutput         Pointer to the stats data output by the algorithm
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*AFProcess)(
        CHIAFAlgorithm*        pAFAlgo,
        const AFAlgoInputList* pInput,
        AFAlgoOutputList*      pOutput);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AFGetParam
    ///
    /// @brief  This method retrieves the most up-to-date AF algorithm information. The output
    ///         is generally updated after calling AFProcess()
    ///
    /// @param  pAFAlgo        Pointer to CHIAFAlgorithm instance
    /// @param  pGetParam      Describes the metadata to query for, and contains any associated data necessary to support
    ///                        that query
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*AFGetParam)(
        CHIAFAlgorithm*       pAFAlgo,
        AFAlgoGetParam*       pGetParam);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AFSetParam
    ///
    /// @brief  Sets input parameters to AF.
    ///
    /// @param  pAFAlgo        Pointer to CHIAFAlgorithm instance
    /// @param  pSetParams     Stats input parameters to set. Each element of the list is of type AFAlgoSetParams
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*AFSetParam)(
        CHIAFAlgorithm*             pAFAlgo,
        const AFAlgoSetParamList*   pSetParams);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AFGetFeatureCapability
    ///
    /// @brief  Gets the supported algorithm features.
    ///
    /// @param  pAFAlgo     Pointer to AFAlgo instance
    /// @param  pFeatures   The features returned from CHIAFAlgorithm, it will be bitmask of supported AFAlgoFeatureType
    ///
    /// @return CDKResultSuccess upon success.
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*AFGetFeatureCapability)(
        CHIAFAlgorithm*   pAFAlgo,
        UINT64*           pFeatures);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AFDestroy
    ///
    /// @brief  This method destroys the derived instance of the interface
    ///
    /// @param  pAFAlgo             Pointer to CHIAFAlgorithm instance
    /// @param  pDestroyParams      Pointer to destroy Param List
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID (*AFDestroy)(
        CHIAFAlgorithm*               pAFAlgo,
        const AFAlgoDestroyParamList* pDestroyParams);

} CHIAFAlgorithm; // CHIAFAlgorithm interface

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateAF
///
/// @brief  This method creates an instance to the CHIAFAlgorithm.
///
/// @param  pCreateParams          Pointer to create params
/// @param  ppAfAlgorithm          Pointer to the created AfAlgorithm instance
///
/// @return CDKResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*CREATEAF)(
    const AFAlgoCreateParamList*   pCreateParams,
    CHIAFAlgorithm**               ppAfAlgorithm);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // CHIAFINTERFACE_H
