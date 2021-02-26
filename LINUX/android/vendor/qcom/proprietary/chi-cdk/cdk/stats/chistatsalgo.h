////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chistatsalgo.h
/// @brief Defines Chi Algorithm interface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHISTATSALGO_H
#define CHISTATSALGO_H

#include "camxcdktypes.h"
#include "chi.h"
#include "chiafinterface.h"
#include "chihafalgorithminterface.h"
#include "chicommon.h"
#include "chiaecinterface.h"
#include "chiawbinterface.h"
#include "chiafdinterface.h"
#include "chiasdinterface.h"
#include <pdlib/chipdlibinterface.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// @brief Pack data structures to ensure consistent layout.
#pragma pack(push, CDK_PACK_SIZE)

typedef struct ChiAlgorithmInterface CHIALGORITHMINTERFACE;
typedef VOID* CHISTATSHANDLE;

/// @brief algorithm capabilities information.
typedef struct ChiAlgorithmCapsInfo
{
    UINT32  size;                ///< Size of this structure
    UINT32  algorithmCapsMask;   ///< algorithm capabilities mask
} CHIALGORITHMCAPSINFO;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNALGORITHMGETCAPABILITIES
///
/// @brief  Get the capabilities supported by the algorithm.
///
/// This function is used to get all the capabilities supported by the algorithm. This information may be used to verify the
/// algorithm properties that are set during algorithm creation, is supported by the algorithm.
///
/// @param  pCapsInfo   Pointer to a structure that defines the capabilities supported by the algorithm.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*PFNALGORITHMGETCAPABILITIES)(CHIALGORITHMCAPSINFO* pCapsInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFCHISETALGORITHMINTERFACE
///
/// @brief  Function to set the chi interfaces required by custom algo.
///
/// This function interface for custom algo to call into Chi.
///
/// @param [in,out] pAlgoInterface  Pointer to a structure that defines callbacks that the algo sends to chi.
///                                 The algo must fill in these function pointers.
///
/// @return VOID.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (*PFCHISETALGORITHMINTERFACE) (CHIALGORITHMINTERFACE* pAlgoInterface);


/// @brief Callback Interface for Chi to call into custom AEC algorithm.
typedef struct ChiAECAlgorithmCallbacks
{
    UINT32                         size;                    ///< Size of this structure.
    UINT32                         majorVersion;            ///< Major version.
    UINT32                         minorVersion;            ///< Minor version.
    PFNALGORITHMGETCAPABILITIES    pfnGetCapabilities;      ///< Get algorithm Capabilities.
    PFNCHIQUERYVENDORTAG           pfnQueryVendorTag;       ///< Optional function to get the vendor tags supported by node.
    CREATEAEC                      pfnCreate;               ///< Mandatory function to create an instance of the algorithm.
    PFCHISETALGORITHMINTERFACE     pfnSetAlgoInterface;     ///< Mandatory function to set the algo interface functions.
} CHIAECALGORITHMCALLBACKS;

/// @brief Callback Interface for Chi to call into custom AF algorithm.
typedef struct ChiAFAlgorithmCallbacks
{
    UINT32                         size;                    ///< Size of this structure.
    UINT32                         majorVersion;            ///< Major version.
    UINT32                         minorVersion;            ///< Minor version.
    PFNALGORITHMGETCAPABILITIES    pfnGetCapabilities;      ///< Get algorithm Capabilities.
    PFNCHIQUERYVENDORTAG           pfnQueryVendorTag;       ///< Optional function to get the vendor tags supported by node.
    CREATEAF                       pfnCreate;               ///< Mandatory function to create an instance of the algorithm.
    CREATEHAFALGORITHM             pfnCreateHAFAlgorithm;   ///< Mandatory function to create an instance of the haf algorithm.
    PFCHISETALGORITHMINTERFACE     pfnSetAlgoInterface;     ///< Mandatory function to set the algo interface functions.

} CHIAFALGORITHMCALLBACKS;

/// @brief Callback Interface for Chi to call into custom AWB algorithm.
typedef struct ChiAWBAlgorithmCallbacks
{
    UINT32                         size;                    ///< Size of this structure.
    UINT32                         majorVersion;            ///< Major version.
    UINT32                         minorVersion;            ///< Minor version.
    PFNALGORITHMGETCAPABILITIES    pfnGetCapabilities;      ///< Get algorithm Capabilities.
    PFNCHIQUERYVENDORTAG           pfnQueryVendorTag;       ///< Optional function to get the vendor tags supported by node.
    CREATEAWB                      pfnCreate;               ///< Mandatory function to create an instance of the algorithm.
    PFCHISETALGORITHMINTERFACE     pfnSetAlgoInterface;     ///< Mandatory function to set the algo interface functions.
} CHIAWBALGORITHMCALLBACKS;

/// @brief Callback Interface for Chi to call into custom AFD ALGORITHM.
typedef struct ChiAFDAlgorithmCallbacks
{
    UINT32                         size;                    ///< Size of this structure.
    UINT32                         majorVersion;            ///< Major version.
    UINT32                         minorVersion;            ///< Minor version.
    PFNALGORITHMGETCAPABILITIES    pfnGetCapabilities;      ///< Get algorithm Capabilities.
    PFNCHIQUERYVENDORTAG           pfnQueryVendorTag;       ///< Optional function to get the vendor tags supported by node.
    CREATEAFD                      pfnCreate;               ///< Mandatory function to create an instance of the algorithm.
    PFCHISETALGORITHMINTERFACE     pfnSetAlgoInterface;     ///< Mandatory function to set the algo interface functions.
} CHIAFDALGORITHMCALLBACKS;

/// @brief Callback Interface for Chi to call into custom ASD algorithm.
typedef struct ChiASDAlgorithmCallbacks
{
    UINT32                         size;                    ///< Size of this structure.
    UINT32                         majorVersion;            ///< Major version.
    UINT32                         minorVersion;            ///< Minor version.
    PFNALGORITHMGETCAPABILITIES    pfnGetCapabilities;      ///< Get algorithm Capabilities.
    PFNCHIQUERYVENDORTAG           pfnQueryVendorTag;       ///< Optional function to get the vendor tags supported by node.
    CREATEASD                      pfnCreate;               ///< Mandatory function to create an instance of the algorithm.
    PFCHISETALGORITHMINTERFACE     pfnSetAlgoInterface;     ///< Mandatory function to set the algo interface functions.
} CHIASDALGORITHMCALLBACKS;

/// @brief Callback Interface for Chi to call into custom PD Library.
typedef struct ChiPDLibraryCallbacks
{
    UINT32                         size;                    ///< Size of this structure.
    UINT32                         majorVersion;            ///< Major version.
    UINT32                         minorVersion;            ///< Minor version.
    PFNALGORITHMGETCAPABILITIES    pfnGetCapabilities;      ///< Get algorithm Capabilities.
    PFNCHIQUERYVENDORTAG           pfnQueryVendorTag;       ///< Optional function to get the vendor tags supported by node.
    CREATEPD                       pfnCreate;               ///< Mandatory function to create an instance of the algorithm.
    PFCHISETALGORITHMINTERFACE     pfnSetAlgoInterface;     ///< Mandatory function to set the algo interface functions.
} CHIPDLIBRARYCALLBACKS;

/// @brief Interface for custom algorithm to call into Chi.
typedef struct ChiAlgorithmInterface
{
    UINT32                          size;                       ///< Size of this structure.
    UINT32                          majorVersion;               ///< Major version.
    UINT32                          minorVersion;               ///< Minor version.
    PFNCHIGETMETADATA               pGetMetadata;               ///< Get the data from metadata if already published.
    PFNCHISETMETADATA               pSetMetaData;               ///< Set the metadata and publish.
    PFNCHIGETVENDORTAGBASE          pGetVendorTagBase;          ///< Get the vendor tag base for a tag.
    PFNCHIQUERYVENDORTAGLOCATION    pQueryVendorTagLocation;    ///< Get Vendor Tag Location
} CHIALGORITHMINTERFACE;

typedef VOID (*PFAECCHIALGORITHMENTRY) (CHIAECALGORITHMCALLBACKS* pAECALGORITHMCallbacks);

typedef VOID (*PFAFCHIALGORITHMENTRY)  (CHIAFALGORITHMCALLBACKS*  pAFALGORITHMCallbacks);

typedef VOID (*PFAWBCHIALGORITHMENTRY) (CHIAWBALGORITHMCALLBACKS* pAWBALGORITHMCallbacks);

typedef VOID (*PFAFDCHIALGORITHMENTRY) (CHIAFDALGORITHMCALLBACKS* pAFDALGORITHMCallbacks);

typedef VOID (*PFASDCHIALGORITHMENTRY) (CHIASDALGORITHMCALLBACKS* pASDALGORITHMCallbacks);

typedef VOID(*PFPDCHILIBRARYENTRY) (CHIPDLIBRARYCALLBACKS* pPDLIBRARYCallbacks);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiAECAlgorithmEntry
///
/// @brief  Entry point called by the Chi driver to initialize the custom AEC Algorithm.
///
/// This function must be exported by every <library>.so in order for driver to initialize the Algorithm. This function is
/// called during the camera server initialization, which occurs during HAL process start. In addition to communicating the
/// necessary function pointers between Chi and external Algorithms, this function allows a Algorithm to do any initialization
/// work that it would typically do at process init. Anything done here should not be specific to a session, and any variables
/// stored in the Algorithm must be protected against multiple sessions accessing it at the same time.
///
/// @param [in,out] pAlgorithmCallbacks  Pointer to a structure that defines callbacks that the Chi driver sends to the
///                                      Algorithm. The Algorithm must fill in these function pointers.
///
/// @return VOID.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC VOID ChiAECAlgorithmEntry(
    CHIAECALGORITHMCALLBACKS* pAlgorithmCallbacks);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiAFAlgorithmEntry
///
/// @brief  Entry point called by the Chi driver to initialize the custom AF Algorithm.
///
/// This function must be exported by every <library>.so in order for driver to initialize the Algorithm. This function is
/// called during the camera server initialization, which occurs during HAL process start. In addition to communicating the
/// necessary function pointers between Chi and external Algorithms, this function allows a Algorithm to do any initialization
/// work that it would typically do at process init. Anything done here should not be specific to a session, and any variables
/// stored in the Algorithm must be protected against multiple sessions accessing it at the same time.
///
/// @param [in,out] pAlgorithmCallbacks  Pointer to a structure that defines callbacks that the Chi driver sends to the
///                                      Algorithm. The Algorithm must fill in these function pointers.
///
/// @return VOID.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC VOID ChiAFAlgorithmEntry(
    CHIAFALGORITHMCALLBACKS* pAlgorithmCallbacks);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiAWBAlgorithmEntry
///
/// @brief  Entry point called by the Chi driver to initialize the custom AWB Algorithm.
///
/// This function must be exported by every <library>.so in order for driver to initialize the Algorithm. This function is
/// called during the camera server initialization, which occurs during HAL process start. In addition to communicating the
/// necessary function pointers between Chi and external Algorithms, this function allows a Algorithm to do any initialization
/// work that it would typically do at process init. Anything done here should not be specific to a session, and any variables
/// stored in the Algorithm must be protected against multiple sessions accessing it at the same time.
///
/// @param [in,out] pAlgorithmCallbacks  Pointer to a structure that defines callbacks that the Chi driver sends to the
///                                      Algorithm. The Algorithm must fill in these function pointers.
///
/// @return VOID.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC VOID ChiAWBAlgorithmEntry(
    CHIAWBALGORITHMCALLBACKS* pAlgorithmCallbacks);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiAFDAlgorithmEntry
///
/// @brief  Entry point called by the Chi driver to initialize the custom AFD Algorithm.
///
/// This function must be exported by every <library>.so in order for driver to initialize the Algorithm. This function is
/// called during the camera server initialization, which occurs during HAL process start. In addition to communicating the
/// necessary function pointers between Chi and external Algorithms, this function allows a Algorithm to do any initialization
/// work that it would typically do at process init. Anything done here should not be specific to a session, and any variables
/// stored in the Algorithm must be protected against multiple sessions accessing it at the same time.
///
/// @param [in,out] pAlgorithmCallbacks  Pointer to a structure that defines callbacks that the Chi driver sends to the
///                                      Algorithm. The Algorithm must fill in these function pointers.
///
/// @return VOID.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC VOID ChiAFDAlgorithmEntry(
    CHIAFDALGORITHMCALLBACKS* pAlgorithmCallbacks);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiASDAlgorithmEntry
///
/// @brief  Entry point called by the Chi driver to initialize the custom ASD Algorithm.
///
/// This function must be exported by every <library>.so in order for driver to initialize the Algorithm. This function is
/// called during the camera server initialization, which occurs during HAL process start. In addition to communicating the
/// necessary function pointers between Chi and external Algorithms, this function allows a Algorithm to do any initialization
/// work that it would typically do at process init. Anything done here should not be specific to a session, and any variables
/// stored in the Algorithm must be protected against multiple sessions accessing it at the same time.
///
/// @param [in,out] pAlgorithmCallbacks  Pointer to a structure that defines callbacks that the Chi driver sends to the
///                                      Algorithm. The Algorithm must fill in these function pointers.
///
/// @return VOID.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC VOID ChiASDAlgorithmEntry(
    CHIASDALGORITHMCALLBACKS* pAlgorithmCallbacks);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiPDLibraryEntry
///
/// @brief  Entry point called by the Chi driver to initialize the custom PD Library.
///
/// This function must be exported by every <library>.so in order for driver to initialize the Algorithm. This function is
/// called during the camera server initialization, which occurs during HAL process start. In addition to communicating the
/// necessary function pointers between Chi and external Algorithms, this function allows a Algorithm to do any initialization
/// work that it would typically do at process init. Anything done here should not be specific to a session, and any variables
/// stored in the Algorithm must be protected against multiple sessions accessing it at the same time.
///
/// @param [in,out] pAlgorithmCallbacks  Pointer to a structure that defines callbacks that the Chi driver sends to the
///                                      Algorithm. The Algorithm must fill in these function pointers.
///
/// @return VOID.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAMX_VISIBILITY_PUBLIC VOID ChiPDLibraryEntry(
    CHIPDLIBRARYCALLBACKS* pAlgorithmCallbacks);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHISTATSALGO_H
