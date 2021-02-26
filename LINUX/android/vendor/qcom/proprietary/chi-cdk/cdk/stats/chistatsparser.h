////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chistatsparser.h
/// @brief Stats Parser external header
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NOWHINE FILE GR016: Typedefs must not be used for structs, classes, and enumerations

#ifndef CHISTATSPARSER_H
#define CHISTATSPARSER_H

#include "chiispstatsdefs.h"
#include "chistatsinterfacedefs.h"

#include "camxcdktypes.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// @brief Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

/// @brief Enumeration of the supported camera architecture families.
typedef enum
{
    CameraFamilyTitan,    ///< Titan camera architecture.
} CHICameraFamily;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Interface for Stats Parser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct CHIStatsParser
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ParseBFStats
    ///
    /// @brief  To get the BF Stats from the buffer.
    ///
    /// @param  pStatsParser Pointer to CHIStatsParser instance.
    /// @param  pBuffer      Pointer to the buffer.
    /// @param  pBFConfig    Pointer to the configuration of the BF Stats.
    /// @param  ppStats      Pointer to the stats buffer.
    ///
    /// @return CDKResultSuccess if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*ParseBFStats)(
        const CHIStatsParser*          pStatsParser,
        VOID*                          pBuffer,
        const BFStatsROIConfigType*    pBFConfig,
        ParsedBFStatsOutput**          ppStats);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ParseHDRBEStats
    ///
    /// @brief  To get the HDRBE Stats from the buffer.
    ///
    /// @param  pStatsParser    Pointer to CHIStatsParser instance.
    /// @param  pBuffer         Pointer to the buffer.
    /// @param  pHDRBEConfig    Pointer to the configuration of the HDRBE Stats.
    /// @param  ppStats         Pointer to the stats buffer.
    ///
    /// @return CDKResultSuccess if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*ParseHDRBEStats)(
        const CHIStatsParser*       pStatsParser,
        VOID*                       pBuffer,
        const BGBEConfig*           pHDRBEConfig,
        ParsedHDRBEStatsOutput**    ppStats);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ParseBEStats
    ///
    /// @brief  To get the AWBBG Stats from the buffer.
    ///
    /// @param  pStatsParser Pointer to CHIStatsParser instance
    /// @param  pBuffer      Pointer to the buffer.
    /// @param  pBGConfig    Pointer to the configuration of the BG Stats.
    /// @param  ppStats      Pointer to the stats buffer.
    ///
    /// @return CDKResultSuccess if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*ParseAWBBGStats)(
        const CHIStatsParser*       pStatsParser,
        VOID*                       pBuffer,
        const BGBEConfig*           pAWBBGConfig,
        ParsedAWBBGStatsOutput**    ppStats);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ParseTintlessBGStats
    ///
    /// @brief  To get the TintlessBG Stats from the buffer.
    ///
    /// @param  pStatsParser      Pointer to CHIStatsParser instance
    /// @param  pBuffer           Pointer to the buffer.
    /// @param  pTintlessBGConfig Pointer to the configuration of the TintlessBG Stats.
    /// @param  ppStats           Pointer to the stats buffer.
    ///
    /// @return CDKResultSuccess if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult(*ParseTintlessBGStats)(
        const CHIStatsParser*         pStatsParser,
        VOID*                         pBuffer,
        const BGBEConfig*             pTintlessBGConfig,
        ParsedTintlessBGStatsOutput** ppStats);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ParseCSStats
    ///
    /// @brief  To get the CS Stats from the buffer.
    ///
    /// @param  pStatsParser    Pointer to CHIStatsParser instance.
    /// @param  pBuffer         Pointer to the buffer.
    /// @param  pCSConfig       Pointer to the configuration of the CS Stats.
    /// @param  ppStats         Pointer to the stats buffer.
    ///
    /// @return CDKResultSuccess if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*ParseCSStats)(
        const CHIStatsParser*      pStatsParser,
        VOID*                      pBuffer,
        const ISPCSStatsConfig*    pCSConfig,
        ParsedCSStatsOutput**      ppStats);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ParseRSStats
    ///
    /// @brief  To get the RS Stats from the buffer.
    ///
    /// @param  pStatsParser    Pointer to CHIStatsParser instance.
    /// @param  pBuffer         Pointer to the buffer.
    /// @param  pRSConfig       Pointer to the configuration of the RS Stats.
    /// @param  ppStats         Pointer to the stats buffer.
    ///
    /// @return CDKResultSuccess if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*ParseRSStats)(
        const CHIStatsParser*      pStatsParser,
        VOID*                      pBuffer,
        const ISPRSStatsConfig*    pRSConfig,
        ParsedRSStatsOutput**      ppStats);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ParseIHistStats
    ///
    /// @brief  To get the IHist Stats from the buffer.
    ///
    /// @param  pStatsParser    Pointer to CHIStatsParser instance.
    /// @param  pBuffer         Pointer to the buffer.
    /// @param  pIHistConfig    Pointer to the configuration of the IHist Stats.
    /// @param  ppStats         Pointer to the stats buffer.
    ///
    /// @return CDKResultSuccess if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult (*ParseIHistStats)(
        const CHIStatsParser*         pStatsParser,
        VOID*                         pBuffer,
        const ISPIHistStatsConfig*    pIHistConfig,
        ParsedIHistStatsOutput**      ppStats);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetMaxStatsBufferSize
    ///
    /// @brief  This gets the Max size for the Parsed and Unparsed stats.
    ///
    /// @param  pStatsParser Pointer to CHIStatsParser instance.
    /// @param  statsType    Type of stats that is needed
    ///
    /// @return size
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT32 (*GetMaxStatsBufferSize)(
        const CHIStatsParser*    pStatsParser,
        ISPStatsType             statsType);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// StatsParserDestroy
    ///
    /// @brief  This method destroys the derived instance of the interface.
    ///
    /// @param  pStatsParser Pointer to CHIStatsParser instance.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID (*StatsParserDestroy)(
        const CHIStatsParser*    pStatsParser);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// context
    ///
    /// @brief  This holds the CHIStatsParser actual instance.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID *pContext;

} CHIStatsParser; // CHIStatsParser interface

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CreateStatsParser
///
/// @brief  This method creates the derived instance of the interface.
///
/// @param  cameraFamily   Allows parser to know for whcih camera family it needs to parse.
/// @param  ppStatsParser  Pointer to CHIStatsParser instance.
///
/// @return CDKResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CreateStatsParser(
    CHICameraFamily     cameraFamily,
    CHIStatsParser**    ppStatsParser);
#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHISTATSPARSER_H