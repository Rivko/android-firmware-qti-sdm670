////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chiafdinterface.h
/// @brief AFD algorithm interfaces
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHIAFDINTERFACE_H
#define CHIAFDINTERFACE_H

#include "chistatsinterfacedefs.h"
#include "chistatsdebug.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

/// @brief AEC Info consumed by AFD
typedef struct
{
    BOOL    isAECsettled;       ///< flag that indicates if aec is settled
    FLOAT   luxIDx;             ///< current lux index
    FLOAT   realGain;           ///< current gain from aec
    FLOAT   currentExpTime;     ///< current exposure time from aec
} AFDAECInfo;

/// @brief Sensor Information consumed by AFD
typedef struct
{
    FLOAT  maxFPS;                  ///< max fps from sensor for this mode
    UINT64 outPixelClock;           ///< Output pixel clock
    UINT   binningTypeH;            ///< Binning horizontal
    UINT   binningTypeV;            ///< Binning vertical
    UINT   numPixelsPerLine;        ///< Number of pixels per line (lineLengthPclk)
    UINT   numLinesPerFrame;        ///< Number of lines per frame (frameLengthLines)
    UINT   maxLineCount;            ///< Max line count
    UINT32 minHorizontalBlanking;   ///< Minimum horizontal blanking interval in pixels
    UINT32 minVerticalBlanking;     ///< Minimum horizontal blanking interval in lines
    FLOAT  rowSumTime;              ///< Row Sum Time
    FLOAT  curFPS;                  ///< Current FPS
} AFDSensorInfo;

/// @brief Auto Focus output information consumed by AFD
typedef struct
{
    BOOL    isAFSearching;     ///< flag that indicates if af is actively searching
} AFDAFInfo;

/// @brief Identifies the type of input parameter to AFD algorithm
typedef enum
{
    AFDAlgoInputInvalid        = -1,        ///< Invalid type
    AFDAlgoInputFrameID,                    ///< The frame number
                                            ///  Payload type: UINT64
    AFDAlgoInputBayerGrid,                  ///< The Bayer stats
                                            ///  Payload type: StatsBayerGrid
    AFDAlgoInputRowSum,                     ///< The row sum stats
                                            ///  Payload type: StatsRowSum
    AFDAlgoInputVendorTag,                  ///< Vendor tag data. List of vendor tags AFD algorithm dependent on
                                            ///  Payload type: StatsVendorTagList
    AFDAlgoInputStatsChiHandle,             ///< Algo needs to use this handle when querying vendor tags
                                            ///  Payload type: CHISTATSHANDLE
    AFDInputTypeLastIndex,                  ///< Last valid index of the enum
                                            ///  Payload type: None
    AFDAlgoInputMax         = 0x7FFFFFFF    ///< Anchor to indicate the last item in the defines
} AFDAlgoInputType;

/// @brief Represents one single AFD inputs to the AFD algorithm
typedef struct
{
    VOID*                   pAFDInput;          ///< Pointer to the payload. See AFDAlgoInputType for details.
    UINT32                  sizeOfAFDInput;     ///< Size of payload
    AFDAlgoInputType        inputType;          ///< Type of the payload
} AFDAlgoInput;

/// @brief Represents AFD module inputs to the AFD algorithm
typedef struct
{
    AFDAlgoInput*       pAFDInputList;          ///< Pointer to the payload. See AFDAlgoInput for details.
    SIZE_T              inputCount;             ///< Number of discrete input
} AFDAlgoInputList;

/// @brief The frame information output by AFD algorithm after processing the statistics
typedef struct
{
    StatisticsAntibanding   currentAFDMode;     ///< AFD Mode detected by Algo
} AFDAlgoFrameSetting;

/// @brief Represents AFD output type
typedef enum
{
    AFDAlgoOutputInvalid       = -1,        ///< Invalid type
    AFDAlgoOutputFrameSetting  = 0,         ///< Frame setting after processing stats
                                            ///  Payload AFDAlgoFrameSetting
    AFDAlgoOutputRowSumConfig,              ///< Row Sum Configuration
                                            ///  AFDAlgoRowSumConfig
    AFDAlgoOutputVendorTag,                 ///< Vendor tag data. List of vendor tags AFD algorithm
                                            ///  wants to publish
                                            ///  Payload StatsVendorTagList
    AFDOutputTypeLastIndex,                 ///< Last OutputType Index
    AFDAlgoLastDetectedMode,                ///< Last detected mode
    AFDAlgoOutputMax           = 0x7FFFFFFF ///< Anchor to indicate the last item in the defines
} AFDAlgoOutputType;

/// @brief Represents the one single type of AFD output from the algorithm
typedef struct
{
    VOID*               pAFDOutput;                 ///< Pointer to the payload. See AFDAlgoOutputType for details
                                                    ///  Algorithm implementer needs to do a deep copy
    UINT32              sizeOfAFDOutput;            ///< Size of input payload pointed to by pAFDOutput
    UINT32              sizeOfWrittenAFDOutput;     ///< Size of payload written back
    AFDAlgoOutputType   outputType;                 ///< Type of the payload
} AFDAlgoOutput;

/// @brief Represents the array of AFD output from the algorithm
typedef struct
{
    AFDAlgoOutput*  pAFDOutputList; ///< Pointer to AFDAlgoOutput array.
    UINT32          outputCount;    ///< Number of elements in pAFDOutputList
} AFDAlgoOutputList;

/// @brief Identifies the type of configuration to AFD algorithm
typedef enum
{
    AFDAlgoSetParamInvalid            = -1,         ///< Invalid type
    AFDAlgoSetParamChromatixData,                   ///< The chromatix tuning header for AFD
                                                    ///  Payload type: ChromatixAFDTuningType
    AFDAlgoSetParamAECInfo,                         ///< AEC updates shared with AFD
                                                    ///  Payload type: AFDAECInfo
    AFDAlgoSetParamAFInfo,                          ///<  AF updates shared with AFD
                                                    ///  Payload type: AFDAFInfo
    AFDAlgoSetParamSensorInfo,                      ///<  Sensor Information update shared with AFD
                                                    ///  Payload type: AFDAFInfo
    AFDAlgoSetParamAFDMode,                         ///< Disable/Enable various AFD Modes
                                                    ///  Payload type: StatisticsAntibanding
    AFDSetParamLastIndex,                           ///< Last valid index of the enum
                                                    ///  Payload: None
    AFDAlgoSetParamMax                = 0x7FFFFFFF  ///< Anchor to indicate the last item in the defines
} AFDAlgoSetParamType;

/// @brief Represents one single AFD configuration
typedef struct
{
    VOID*               pAFDSetParam;      ///< Pointer to AFD set param data
                                           ///  See AFDAlgoSetParamType for details
    UINT32              sizeOfAFDSetParam; ///< Size of data pointed to by pAFDSetParam
    AFDAlgoSetParamType setParamType;      ///< Type of the input parameter
} AFDAlgoSetParam;

/// @brief Represents the list of AFD configuration parameters passed to AFDSetParam
typedef struct
{
    AFDAlgoSetParam*    pAFDSetParamList;    ///< Pointer to AFD set param data
                                             ///  See AFDAlgoSetParam for details
    SIZE_T              numberOfSetParam;    ///< Number of input param parameters
} AFDAlgoSetParamList;

/// @brief Represents AFD stats configuration required from algo
typedef struct
{
    UINT32  statsHnum;   ///< Max number of Horizontal Rows
    UINT32  statsVnum;   ///< Max number of Vertical Columns
} AFDAlgoRowSumConfig;

/// @brief Identifies the type of query to AFD algorithm
typedef enum
{
    AFDAlgoGetParamInvalid                     = -1,           ///< The invalid type
    AFDAlgoGetParamStatsConfig,                                ///< Get the afd stats config from algo
                                                               ///  Input payload: none
                                                               ///  Output payload: AFDAlgoStatsConfig
    AFDAlgoGetParamPublishingVendorTagsInfo,                   ///< The vendor tags published by the algorithm
                                                               ///  Input payload: none
                                                               ///  Output payload: AFDAlgoGetParamOutput include following type
                                                               ///    AFDAlgoGetParamOutputVendorTagInfoList
    AFDAlgoGetParamDependentVendorTags,                        ///< The vendor tags the algorithm depends on
                                                               ///  Input payload: NULL
                                                               ///  Output payload: AFDAlgoGetParamOutput include following type
                                                               ///    AFDAlgoGetParamOutputVendorTagInfoList
    AFDAlgoGetParamOutputVendorTagInfoList,                    ///< The vendor tag info list
                                                               ///  Input payload: NULL
                                                               ///  Payload StatsVendorTagInfoList
    AFDAlgoGetParamLastDetectedMode,                           ///< Get last detected AFD mode from algo instance
                                                               ///  Input payload: none
                                                               ///  Payload AFDAlgoLastDetectedMode
    AFDAlgoGetParamMax                         = 0x7FFFFFFF    ///< Anchor to indicate the last item in the defines
} AFDAlgoGetParamType;

/// @brief Represents the metadata to query for, and contains any associated data necessary to support that query
typedef struct
{
    VOID*   pInputData;         ///< Pointer to input data given in the query metadata
                                ///  to generate the output
    UINT32  sizeOfInputData;    ///< Size of input data pointered to by pInputData
} AFDAlgoGetParamInput;

/// @brief Represents the output for the metadata to query for
typedef struct
{
    VOID*   pOutputData;        ///< Pointer to output data for which queried the metadata. See
                                ///  AFDAlgoGetParamType for details
    UINT32  sizeOfOutputData;   ///< Size of output data pointed to by pOutputData
} AFDAlgoGetParamOutput;

/// @brief Represents the structure to query information from algorithm
typedef struct
{
    AFDAlgoGetParamType    type;   ///< The type of information to get from algorithm
    AFDAlgoGetParamInput   input;  ///< The input used to indicate the detail information to get
    AFDAlgoGetParamOutput  output; ///< The output of queried information
} AFDAlgoGetParam;

/// @brief Represents the structure containing list of param to query algorithm
typedef struct
{
    AFDAlgoGetParam*    pAFDGetParamList;       ///< Pointer to AFD get param data array
    UINT32              numberOfAFDGetParams;   ///< Number of param parameters
} AFDAlgoGetParamList;


/// @Represents the AFD features supported by the algorithm
typedef enum
{
    AFDAlgoFeatureBasic        = 1 << 0,        ///< Basic AFD Features - with basic functionalities
    AFDAlgoFeatureMax          = 1 << 31,       ///< Anchor to indicate the last item in the defines
} AFDAlgoFeatureType;

/// @brief Represents one single AFD create parameters used to create the AFD algorithm instance
typedef enum
{
    AFDAlgoCreateParamsInvalid              = -1,        ///< Invalid type
    AFDAlgoCreateParamsLoggerFunctionPtr,                ///< Logging function pointer
                                                         ///  Payload type: StatsLoggerFunction
    AFDCreateParamTypeCount,                             ///< Create Param Type Count
    AFDAlgoCreateParamsMax                  = 0x7FFFFFFF ///< Anchor to indicate the last item in the defines
} AFDAlgoCreateParamsType;

/// @brief Represents one single AFD create parameters used to create the AFD algorithm instance
typedef struct
{
    VOID*                        pCreateParam;       ///< Payload. See AFDAlgoCreateParamType for details
    UINT32                       sizeOfCreateParam;  ///< Size of payload pointed by data.
    AFDAlgoCreateParamsType      createParamType;    ///< Create parameter type.
} AFDAlgoCreateParam;

/// @brief Represents AFD create parameter list used to create the AFD algorithm instance
typedef struct
{
    AFDAlgoCreateParam*          pCreateParamList;      ///< Payload. See AFDAlgoCreateParamType for details
    SIZE_T                       paramCount;            ///< Number of create parameters.
} AFDAlgoCreateParamList;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Interface for AFD algorithm.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct CHIAFDAlgorithm
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AFDDestroy
    ///
    /// @brief  This method destroys the derived instance of the interface
    ///
    /// @param  pCHIAFDAlgorithm   Pointer to CHIAFDAlgorithm instance
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID (*AFDDestroy)(
        CHIAFDAlgorithm*   pCHIAFDAlgorithm);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AFDProcess
    ///
    /// @brief  Runs the statistic algorithm on the given inputs. This method must only be called after the algorithm is
    ///         initialized.
    ///
    /// @param  pCHIAFDAlgorithm   Pointer to CHIAFDAlgorithm instance
    /// @param  pInputList         Pointer to stats inputs interface
    /// @param  pOutputList        Pointer to the stats data output by the algorithm
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*AFDProcess)(
        CHIAFDAlgorithm*             pCHIAFDAlgorithm,
        const AFDAlgoInputList*      pInputList,
        AFDAlgoOutputList*           pOutputList);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AFDGetParam
    ///
    /// @brief  This method retrieves the most up-to-date AFD algorithm metadata. The output
    ///         is generally updated after calling CHIAFDAlgorithm::AFDProcess()
    ///
    /// @param  pCHIAFDAlgorithm   Pointer to CHIAFDAlgorithm instance
    /// @param  pGetParam          Contains the list of param to query for, and contains any associated data necessary to support
    ///                            that query
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*AFDGetParam)(
        CHIAFDAlgorithm*      pCHIAFDAlgorithm,
        AFDAlgoGetParamList*  pGetParamList);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AFDSetParam
    ///
    /// @brief  Sets configuration parameters to AFD.
    ///
    /// @param  pCHIAFDAlgorithm  Pointer to CHIAFDAlgorithm instance
    /// @param  pSetParamList     Stats configuration parameters to set.Each element of the list is of type AFDAlgoSetParam
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*AFDSetParam)(
        CHIAFDAlgorithm*                pCHIAFDAlgorithm,
        const AFDAlgoSetParamList*      pSetParamList);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AFDGetFeatureCapability
    ///
    /// @brief  Gets the supported algorithm features.
    ///
    /// @param  pCHIAFDAlgorithm  Pointer to CHIAFDAlgorithm instance
    /// @param  pFeatures         The features returned from CHIAFDAlgorithm, it will be bitmask of supported AFDAlgoFeatureType
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*AFDGetFeatureCapability)(
        CHIAFDAlgorithm*   pCHIAFDAlgorithm,
        UINT64*            pFeatures);

} CHIAFDAlgorithm; // CHIAFDAlgorithm interface

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateAFD
///
/// @brief  This method creates an instance to the CHIAFDAlgorithm.
///
/// @param  pCreateParamList        Pointer to create param list
/// @param  ppCHIAFDAlgorithm       Pointer to the created CHIAFDAlgorithm instance
///
/// @return CDKResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*CREATEAFD)(
    const AFDAlgoCreateParamList*      pCreateParamList,
    CHIAFDAlgorithm**                  ppCHIAFDAlgorithm);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHIAFDINTERFACE_H
