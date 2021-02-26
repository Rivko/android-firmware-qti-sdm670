////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chitintlessinterface.h
///
/// @brief Qualcomm Technologies, Inc. Tintless Algorithm Interface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHITINTLESSINTERFACE_H
#define CHITINTLESSINTERFACE_H

#include "camxcdktypes.h"

#include "chiispstatsdefs.h"

#ifdef __cplusplus

extern "C"
{
#endif // __cplusplus

/// @brief Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

/// @brief Number of Tintless threshold entries
#define TINTLESS_THRESHOLD_ENTRIES 16
#define MAX_STATS_NUM 3072    ///< Maximum nuber of stats entries 64 x 48, but tintless algo support only 32 x 24
#define ROLLOFF_SIZE  221      ///< 17 x 13 entries per mesh table

/// @brief Represents stats configuration parameters passed to ITintlessAlgorithm::Configure()
struct StatsConfigParams
{
    UINT32            camifWidth;                     ///< Input image width
    UINT32            camifHeight;                    ///< Input image Height
    UINT32            statsRegionWidth;               ///< Region width of stats
    UINT32            statsRegionHeight;              ///< Region height of stats
    UINT32            statsNumberOfHorizontalRegions; ///< Total number of horizontal regions
    UINT32            statsNumberOfVerticalRegions;   ///< Total number of vertical regions
    UINT32            postBayer;                      ///< HRDBE/TintlessBG postBayer = 0/ AWBBG postBayer = 1
    UINT32            saturationLimit[ChannelIndexCount];                ///< Saturation limit for stats
    UINT32            statsBitDepth;                  ///< Bit depth of stats
};

/// @brief Represents LSC configuration parameters
struct LSCConfigParams
{
    UINT32 LSCTableWidth;              ///< Mesh rolloff table width
    UINT32 LSCTableHeight;             ///< Mesh rolloff table height
    UINT32 LSCSubgridWidth;            ///< Mesh rolloff subgrid width
    UINT32 LSCSubgridHeight;           ///< Mesh rolloff subgrid height
    UINT32 numberOfLSCSubgrids;        ///< Number of mesh rolloff sub grids
    UINT32 LSCSubgridHorizontalOffset; ///< Sub grid horizontal offset
    UINT32 LSCSubgridVerticalOffset;   ///< Sub grid vertical offset
};

/// @brief Represents Tintless configuration parameters
struct TintlessConfigParams
{
    UINT8 tintlessCorrectionStrength;                    ///< Tintless Algorithm Correction strength
    UINT8 tintlessThreshold[TINTLESS_THRESHOLD_ENTRIES]; ///< Tintless Threshold
    UINT8 tintAcuuracy;                                  ///< Tint correction accuracy
    UINT8 updateDelay;                                   ///< Tintless update Delay
    FLOAT tracePercentage;                               ///< Tintless trace percentage
    FLOAT centerWeight;                                  ///< Tintless correction center weight
    FLOAT cornerWeight;                                  ///< Tintless correction corner weight
    UINT8 applyTemporalFiltering;                        ///< 1 - apply Temporal Filtering 0 - dont apply
};

/// @brief Tintless Mesh rolloff Table
struct TintlessRolloffTable
{
    UINT16 TintlessRoloffTableSize; ///< Tintless Rolloff table size
    FLOAT* RGain;                   ///< R Channel gains for Tintless table
    FLOAT* GRGain;                  ///< Gr Channel gains for Tintless table
    FLOAT* GBGain;                  ///< Gb Channel gains for Tintless table
    FLOAT* BGain;                   ///< B Channel gains for Tintless table
};

/// @brief Tintless Config
struct TintlessConfig
{
    LSCConfigParams      rolloffConfig;
    StatsConfigParams    statsConfig;
    TintlessConfigParams tintlessParamConfig;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Interface for Tintless algorithm.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// TintlessAlgorithmProcess
    ///
    /// @brief  Process the Input Table and generate the Output table
    ///
    /// @param  pChiTintlessAlgo  Pointer to the CHI tintless interface
    /// @param  pTintlessConfig   Pointer to Tintless Algo Config.
    /// @param  pTintlessStats    Pointer to the Tintless Stats data
    /// @param  PInputTable       Pointer to the Input Rolloff Table
    /// @param  POutputTable      Pointer to the Output Rolloff Table
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult(*TintlessAlgorithmProcess)(
        VOID*                              pChiTintlessAlgo,
        const TintlessConfig*              pTintlessConfig,
        const ParsedTintlessBGStatsOutput* pTintlessStats,
        TintlessRolloffTable*              pInputTable,
        TintlessRolloffTable*              pOutputTable);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// TintlessDestroy
    ///
    /// @brief  This method destroys the derived instance of the interface
    ///
    /// @param  pChiTintlessAlgo  Pointer to the CHI tintless interface
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID(*TintlessDestroy)(VOID* pChiTintlessAlgo);

    VOID* pTintlessAlgoResource; ///< Tintless Algorithm Resource
} CHITintlessAlgorithm;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateTintless
///
/// @brief  This function pointer to method of creating a handle of CHITintlessAlgorithm.
///
/// @param  ppCHITintlessAlgorithm  Pointer to pointer of the created CHITintlessAlgorithm instance
///
/// @return CDKResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*CREATETINTLESS)(
    CHITintlessAlgorithm** ppCHITintlessAlgorithm);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHITINTLESSINTERFACE_H
