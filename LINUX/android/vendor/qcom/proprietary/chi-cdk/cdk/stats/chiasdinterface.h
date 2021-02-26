////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chiasdinterface.h
/// @brief Auto Scene Detection algorithm interfaces
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHIASDINTERFACE_H
#define CHIASDINTERFACE_H

#include "chistatsdebug.h"
#include "chistatsinterfacedefs.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

static const CHAR* VendorTagSectionASDResults    = "org.quic.camera2.asdresults";   ///< ASD results vendor tag section
static const CHAR* VendorTagNameASDResults       = "ASDResults";                    ///< ASD rsults vendor tag name

/// @brief The Backlight information output by ASD algorithm after processing the statistics
typedef struct
{
    BOOL    backlightDetected;      ///< Backlight detected or not.
    BOOL    histoBacklightDetected; ///< Histo backlight detected or not.
    UINT32  backlightSceneSeverity; ///< Backlight Scene Severity.
    BOOL    mixedLight;             ///< Flag that indicates the light is mixed or not.
    BOOL    snowSceneDetected;      ///< Snow/Cloudy scene detected or not.
    UINT32  landscapeSeverity;      ///< Landscape Severity, value range from 0 to 255.
    UINT32  portraitSeverity;       ///< Portrait Severity, value range from 0 to 255.
    BOOL    isHdrScene;             ///< HDR scene or not.
    FLOAT   hdrConfidence;          ///< The HDR confidence,value ranges from 0.0f to 1.0f.
}ASDAlgoSceneInfo;

/// @brief Represents ASD output type
typedef enum
{
    ASDAlgoOutputInvalid       = -1,         ///< Invalid type
    ASDAlgoOutputSceneInfo     = 0,          ///< Scene info.
                                             ///  Payload: ASDAlgoSceneInfo
    ASDAlgoOutputDebugData,                  ///< Debug data used
                                             ///  Payload: StatsDataPointer
    ASDAlgoOutputVendorTag,                  ///< Vendor tag data. List of vendor tags the algorithm wants to publish.
                                             ///  Payload StatsVendorTagList
    ASDAlgoOutputCount,                      ///< Indicate the total number of algorithm output
    ASDAlgoOutputMax           = 0x7FFFFFFF  ///< Anchor to indicate the last item in the defines
}ASDAlgoOutputType;

/// @brief Represents the one single type of ASD output from the algorithm
typedef struct
{
    VOID*                   pASDOutput;                 ///< Pointer to the payload. See ASDAlgoOutputType for details
                                                        ///  Algorithm implementer needs to do a deep copy
    SIZE_T                  sizeOfASDOutput;            ///< Size of input payload pointed to by pASDOutput
    SIZE_T                  sizeOfWrittenASDOutput;     ///< Size of payload written back
    ASDAlgoOutputType       outputType;                 ///< Type of the payload
}ASDAlgoOutput;

/// @brief Represents the array of ASD output from the algorithm
typedef struct
{
    ASDAlgoOutput* pASDOutputList;    ///< Pointer to ASDAlgoOutput array.
    SIZE_T         numberOfOutput;    ///< Number of elements in pOutputData
}ASDAlgoOutputList;

/// @brief This structure defines the format of the Exposure information input/AWB input info into the ASD algorithm.
typedef struct
{
    StatsLumaSum    SYData;                 ///< Containing SY data.
    UINT32          numRegion;              ///< Number of regions.
    UINT32          lumaTarget;             ///< Target luma.
    FLOAT           luxIndex;               ///< Lux index.
    FLOAT           asdExtremeGreenRatio;   ///< The extreme color green ratio.
    FLOAT           asdExtremeBlueRatio;    ///< The extreme color blue ratio.
    UINT32          awbSampleDecisionCount; ///< Number of AWB sample decision.
    INT32*          awbSampleDecision;      ///< Pointer to array of Max size 64.
    INT32           awbFinaldecision;       ///< AWB Final Decision.
}ASDAlgoAECAWBInfo;

/// @brief Identifies the type of input parameter to ASD algorithm
typedef enum
{
    ASDAlgoInputInvalid        = -1,            ///< Invalid type
    ASDInputRequestID          =  0,            ///< Request ID type.
                                                ///  Payload type: UINT64
    ASDAlgoInputBayerHist,                      ///< The Bayer histogram
                                                ///  Payload type: StatsBayerHist
    ASDAlgoInputFaceInfo,                       ///< The Face data
                                                ///  Payload type: StatsFaceInformation
    ASDAlgoInputAECAWBData,                     ///< AEC/AWB data input type
                                                ///  Payload type: ASDAlgoAECAWBInfo
    ASDAlgoInputVendorTag,                      ///< Vendor tag data. List of vendor tags the algori
                                                ///< thm dependents on.
                                                ///  Payload type: StatsVendorTagList
    ASDAlgoInputStatsChiHandle,                 ///< Algo needs to use this handle when querying vendor tags
                                                ///  Payload type: CHISTATSHANDLE
    ASDAlgoInputCount,                          ///< Indicate the total number of algorithm input
    ASDAlgoInputMax            = 0x7FFFFFFF     ///< Anchor to indicate the last item in the defines
}ASDAlgoInputType;

/// @brief Represents one single ASD input to the ASD algorithm
typedef struct
{
    VOID*               pASDInput;      ///< Pointer to the payload. See ASDAlgoInputType for details.
    UINT32              sizeOfASDInput; ///< Size of payload
    ASDAlgoInputType    inputType;      ///< Type of the payload
}ASDAlgoInput;

/// @brief Represents ASD module inputs to the ASD algorithm
typedef struct
{
    ASDAlgoInput*  pASDInputList;         ///< Pointer to the payload. See ASDAlgoInput for details.
    SIZE_T         numberOfASDInput;      ///< Number of discrete input
}ASDAlgoInputList;

/// @brief Defines the cam auto scene type.
typedef enum
{
    ASDAlgoAutoSceneInvalid             = -1,
    ASDAlgoAutoSceneNormal              =  0,
    ASDAlgoAutoSceneScenery,
    ASDAlgoAutoScenePortrait,
    ASDAlgoAutoScenePortraitBacklight,
    ASDAlgoAutoSceneSceneryBacklight,
    ASDAlgoAutoSceneBacklight,
    ASDAlgoAutoSceneHDR,
    ASDSceneTypeDefaultMax              = 10,
    ASDAlgoAutoSceneCustom0             = ASDSceneTypeDefaultMax,
    ASDAlgoAutoSceneCustom1,
    ASDAlgoAutoSceneCustom2,
    ASDAlgoAutoSceneCustom3,
    ASDAlgoAutoSceneCustom4,
    ASDAlgoAutoSceneCustom5,
    ASDAlgoAutoSceneCustom6,
    ASDAlgoAutoSceneCustom7,
    ASDAlgoAutoSceneCustom8,
    ASDAlgoAutoSceneCustom9,
    ASDSceneTypeCustomMax,
}ASDAlgoAutoSceneType;

/// @brief Identifies the type of input parameter to AWB algorithm
typedef enum
{
    ASDAlgoSetParamInvalid         = -1,         ///< Invalid type
    ASDAlgoSetParamChromatixData,                ///< The chromatix tuning header for ASD
                                                 ///  Payload type: StatsTuningData
    ASDAlgoSetParamAsdEnable,                    ///< To Enable ASD.
                                                 ///  Payload type: BOOL
    ASDAlgoSetParamFrameDim,                     ///< Frame Dimension
    ASDSetParamLastIndex,                        ///< Last valid index of the enum
                                                 ///  Payload: None
    ASDAlgoSetParamMax             = 0x7FFFFFFF  ///< Anchor to indicate the last item in the defines
}ASDAlgoSetParamType;

/// @brief Represents an AWB discreate input parameter
typedef struct
{
    VOID*                  pASDSetParam;         ///< Pointer to ASD input param data
                                                 ///  See ASDAlgoSetParamType for details
    UINT32                 sizeOfASDSetParam;    ///< Size of data pointed to by pASDInputParam
    ASDAlgoSetParamType    setParamType;         ///< Type of the input parameter
}ASDAlgoSetParam;


/// @brief Represents an AWB input parameter passed to AWBAlgorithm::SetInput()
typedef struct
{
    ASDAlgoSetParam*   pASDSetParamList;         ///< Pointer to ASD input param data
                                                 ///  See ASDAlgoSetParamType for details
    SIZE_T             numberOfASDSetParam;      ///< Number of input param parameters
}ASDAlgoSetParamList;

/// @brief Identifies Severities for different scenes.
typedef struct
{
    UINT32  landscapeSeverity;  ///< Degree of landscape content in the scene
    UINT32  backlightDetected;  ///< True if backlight detected
    UINT32  backlightSeverity;  ///< Degree by which backlight adjustment needs to be done
    UINT32  portraitSeverity;   ///< Degree of portrait content in the scene
    FLOAT   softFocusDeg;       ///< Degree of softness to be applied for portrait
} ASDGetParamSceneInfo;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Describe ASD Algorithm Get Parameter definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Indicates the type of ASD Get/Query of parameters
typedef enum
{
    ASDAlgoGetParameterTypeInvalid = -1,        ///< Invalid type
                                                ///  Payload: NULL & set size to zero.
    ASDAlgoGetParameterTypeSceneInfo,            ///< Queries the ASD scene info for landscape and portrait.
                                                ///  Payload in: NULL. Payload out: ASDGetParamSeverity
    ASDAlgoGetParamPublishingVendorTagsInfo,    ///< The vendor tags published by the algorithm
                                                ///  Payload in: NULL
                                                ///  Payload out: ASDAlgoGetParamOutputs include following type
    ASDAlgoGetParamDependentVendorTags,         ///< The vendor tags the algorithm depends on Input
                                                ///  Payload in: NULL
                                                ///  Payload out: ASDAlgoGetParamOutputs include following type
    ASDAlgoGetParamCount,                       ///< Indicate the total number of get param to algorithm
    ASDAlgoGetParameterTypeMax     = 0x7FFFFFFF
}ASDAlgoGetParamType;

/// @brief Represents the metadata to query for, and contains any associated data necessary to support that query
typedef struct
{
    VOID*   pInputData;         ///< Pointer to input data given in the query metadata
                                ///  to generate the output.
    UINT32  sizeOfInputData;    ///< Size of input data pointed to by pInputData
}ASDAlgoGetParamInputs;

/// @brief Represents the output for the metadata to query for
typedef struct
{
    VOID*   pOutputData;        ///< Pointer to output data for which queried the metadata.
    UINT32  sizeOfOutputData;   ///< Size of output data pointed to by pOutputData.
}ASDAlgoGetParamOutputs;

/// @brief Represents the structure to query information from ASD algorithm
typedef struct
{
    ASDAlgoGetParamType        type;       ///< Type of the metadta query
    ASDAlgoGetParamInputs      inputs;     ///< The input used to indicate the detail information to get
    ASDAlgoGetParamOutputs     outputs;    ///< The outputs of queried information
}ASDAlgoGetParam;

/// @Represents the ASD features supported by the algorithm
typedef enum
{
    ASDAlgoFeatureBasic = 1 << 0,        ///< Basic ASD Features - with basic functionalities
    ASDAlgoFeatureMax   = 1 << 31,       ///< Anchor to indicate the last item in the defines
} ASDAlgoFeatureType;

/// @Represents ASD create parameters information necessary to create ASD algorithm. See CreateASD.
typedef enum
{
    ASDAlgoCreateParamsInvalid              = -1,        ///< Invalid type
    ASDAlgoCreateParamsLoggerFunctionPtr,                ///< Logging function pointer
                                                         ///  Payload type: StatsLoggerFunction
    ASDCreateParamTypeCount,                             ///< Create Param Type Count
    ASDAlgoCreateParamsMax                  = 0x7FFFFFFF ///< Anchor to indicate the last item in the defines
}ASDAlgoCreateParamsType;

/// @brief Represents one single ASD create parameters used to create the ASD algorithm instance
typedef struct
{
    VOID*                        pCreateParam;       ///< Payload. See ASDAlgoCreateParamType for details
    UINT32                       sizeOfCreateParam;  ///< Size of payload pointed by data.
    ASDAlgoCreateParamsType createParamType;         ///< Create parameter type.
}ASDAlgoCreateParam;

/// @brief Represents ASD create parameter list used to create the ASD algorithm instance
typedef struct
{
    ASDAlgoCreateParam*     pCreateParamList;        ///< Payload. See ASDAlgoCreateParamType for details
    SIZE_T                  numberOfCreateParam;     ///< Number of create parameters.
}ASDAlgoCreateParamList;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Interface for ASD algorithm.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct CHIASDAlgorithm
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ASDProcess
    ///
    /// @brief  Runs the statistic algorithm on the given inputs. This method must only be called after the algorithm is
    ///         initialized.
    ///
    /// @param  pCHIASDAlgorithm   Pointer to CHIASDAlgorithm instance
    /// @param  pInput             Pointer to stats inputs interface
    /// @param  pOutput            Pointer to the stats data output by the algorithm
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*ASDProcess)(
        CHIASDAlgorithm*            pCHIASDAlgorithm,
        const ASDAlgoInputList*     pInputs,
        ASDAlgoOutputList*          pOutputs);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ASDGetParam
    ///
    /// @brief  This method retrieves the most up-to-date ASD algorithm metadata. The output
    ///         is generally updated after calling CHIASDAlgorithm::ASDProcess()
    ///
    /// @param  pCHIASDAlgorithm   Pointer to CHIASDAlgorithm instance
    /// @param  pGetParam          Describes the metadata to query for, and contains any associated data necessary to support
    ///                            that query
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*ASDGetParam)(
        CHIASDAlgorithm*   pCHIASDAlgorithm,
        ASDAlgoGetParam*   pGetParam);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ASDSetParam
    ///
    /// @brief  Sets configuration parameters to ASD.
    ///
    /// @param  pCHIASDAlgorithm  Pointer to CHIASDAlgorithm instance
    /// @param  pSetParams        Stats configuration parameters to set.Each element of the list is of type AECAlgorithmInputParams
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*ASDSetParam)(
        CHIASDAlgorithm*              pCHIASDAlgorithm,
        const ASDAlgoSetParamList*    pSetParams);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ASDGetFeatureCapability
    ///
    /// @brief  Get ASD Feature Capability.
    ///
    /// @param  pCHIASDAlgorithm   Pointer to CHIASDAlgorithm instance
    /// @param  pFeatures          Pointer to ASD Features.
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*ASDGetFeatureCapability)(
        CHIASDAlgorithm*   pCHIASDAlgorithm,
        UINT64*            pFeatures);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ASDDestroy
    ///
    /// @brief  This method destroys the derived instance of the interface
    ///
    /// @param  pCHIASDAlgorithm   Pointer to AECAlgorithm instance
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID(*ASDDestroy)(
        CHIASDAlgorithm*   pCHIASDAlgorithm);

}CHIASDAlgorithm; // CHIASDAlgorithm interface

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateASD
///
/// @brief  This function pointer to method creates an instance to the CHIASDAlgorithm.
///
/// @param  pCreateParams       Pointer to create params
/// @param  ppCHIASDAlgorithm   Pointer to the created CHIASDAlgorithm instance
///
/// @return CDKResultSuccess upon success.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*CREATEASD)(
    const ASDAlgoCreateParamList*    pCreateParams,
    CHIASDAlgorithm**                ppCHIASDAlgorithm);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHIASDINTERFACE_H
