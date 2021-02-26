////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file camxtuningdatamanager.h
/// @brief Class and API-s to interact with chromatix/tuning data manager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXTUNINGDATAMANAGER_H
#define CAMXTUNINGDATAMANAGER_H

#include "camxdefs.h"
#include "camxtypes.h"

/// Autogen file includes
/// @todo (CAMX-829) Fix warnings in auto-gen code
#if defined (_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4100)               // NOWHINE PR006 <- Temporary disable for autogen code
#elif defined (__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif // _MSC_VER

#include "parametertuningtypes.h"
#include "tuningsetmanager.h"

#if defined (_MSC_VER)
#pragma warning(pop)
#elif defined (__GNUC__)
#pragma GCC diagnostic pop
#endif // _MSC_VER

CAMX_NAMESPACE_BEGIN

/// Constants
static const UINT  MaxTunedNodes = 16;   ///< Max number of override modes a module can be tuned for
static const UINT  MaxTunedModes = 4;    ///< Max number of mode selectors for a tuning override mode of a module

/// Forward declarations
struct MatchParams;
struct TunedNode;
struct TunedModule;
struct TunedModulesInfo;
struct TunedBufferData;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Class containing APIs to interact with chromatix/tuning data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TuningDataManager
{
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create
    ///
    /// @brief  Static method to create an instance of TuningDataManager
    ///
    /// @return Instance pointer to be returned or NULL
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static TuningDataManager* Create();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Destroy
    ///
    /// @brief  Method to delete an instance of TuningDataManager
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Destroy();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LoadTunedDataFile
    ///
    /// @brief  Load and pre-parse a tuned data file corresponding to a detected sensor
    ///
    /// @param  pFilename   File name to open
    ///
    /// @return CamxResultSuccess, if SUCCESS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult LoadTunedDataFile(
        const CHAR* pFilename);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CreateTunedModeTree
    ///
    /// @brief  Create tuning mode tree for all modules for the detected sensor
    ///
    /// @return CamxResultSuccess, if SUCCESS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult CreateTunedModeTree();
     ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CreateTunedModeTree
    ///
    /// @brief  Create tuning mode tree for all modules for the detected sensor
    ///
    /// @return CamxResultSuccess, if SUCCESS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult UpdateTunedModeTree();
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CheckAndFreeTunedBuf
    ///
    /// @brief  Free Tuned Buffer
    ///
    /// @return CamxResultSuccess, if SUCCESS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult CheckAndFreeTunedBuf();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetChromatix
    ///
    /// @brief  Create tuning mode tree for all modules for the detected sensor
    ///
    /// @return TuningSetManager*, if SUCCESS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TuningSetManager* GetChromatix();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsValidChromatix
    ///
    /// @brief  Checks if call to GetChormatix is valid or not
    ///
    /// @return true, if
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL IsValidChromatix();
    BOOL IsChanged();
    BOOL IsModified();
    CamxResult setIsChanged(BOOL);
protected:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetTunedModule
    ///
    /// @brief  Fetch the core structure representing a tuned module, corresponding to a set of tuning modes
    ///         requested by the client.
    ///         This function will not be called directly by the clients of TuningDataManager. Instead, they
    ///         will call the public typesafe methods of TunedParameterManager (from which TuningDataManager
    ///         inherits) for getting the particular module structure they are interested in. These methods
    ///         include, but is not an exhaustive list, the following -
    ///         1. GetTuned_chromatix_abf34Type
    ///         2. GetTuned_chromatix_demux13Type
    ///         3. GetTuned_chromatix_demux13Type, etc.
    ///
    /// @param  pModuleName        Name of the module to be fetched
    /// @param  pRequestedModes    The set of mode selectors the module is expected to be tuned for
    /// @param  numRequestedModes  The number of modes selectors in the set
    /// @param  ppMatchingModule   Out pointer to the fetched module
    ///
    /// @return CamxResultSuccess, if SUCCESS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult GetTunedModule(
        const CHAR*       pModuleName,
        TuningMode*       pRequestedModes,
        UINT              numRequestedModes,
        ParameterModule** ppMatchingModule);

private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// TuningDataManager
    ///
    /// @brief  Constructor for TuningDataManager object
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    TuningDataManager();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~TuningDataManager
    ///
    /// @brief  Destructor for TuningDataManager object
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ~TuningDataManager();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize
    ///
    /// @brief  Initialize internal data structures for the TuningDataManager
    ///
    /// @return CamxResultSuccess, if SUCCESS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult Initialize();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LoadFile
    ///
    /// @brief  Load a tuned data file to memory
    ///
    /// @param  pFilename       Name to open
    /// @param  ppBuffer        Pointer to memory, where the contents of the file will be loaded
    /// @param  pBufferLength   Returned number of bytes loaded
    ///
    /// @return CamxResultSuccess, if SUCCESS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult LoadFile(
        const CHAR* pFilename,
        BYTE**      ppBuffer,
        UINT64*     pBufferLength);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// MatchRequestedModes
    ///
    /// @brief  Given a requested set of mode selectors, find the closest match in a set of tuned modes for a module
    ///
    /// @param  pModule       Data structure representing a module and its mode tree
    /// @param  pSelectors    The set of mode selectors the module is expected to be tuned for
    /// @param  numSelectors  The number of modes selectors in the set
    ///
    /// @return Autogen ParameterModule pointer for the best matched node in the mode tree
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ParameterModule* MatchRequestedModes(
        TunedModule* pModule,
        TuningMode*  pSelectors,
        UINT         numSelectors);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetNodeMatchPotential
    ///
    /// @brief  Given a requested set of mode selectors, find the match potential of a node in the mode tree of a module
    ///
    /// @param  pTunedNode           Data structure representing a node in a mode tree
    /// @param  pMatchParams         Output data structure to fill the match credentials for the node into
    /// @param  pRequestSelectors    The set of mode selectors the module is expected to be tuned for
    /// @param  numRequestSelectors  The number of modes selectors in the set
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID GetNodeMatchPotential(
        TunedNode*   pTunedNode,
        MatchParams* pMatchParams,
        TuningMode*  pRequestSelectors,
        UINT         numRequestSelectors);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CopyTuningModeInfo
    ///
    /// @brief  Copy the mode, selector and selector data for a node in the mode tree of a module
    ///
    /// @param  pTuningMode   Mode information, of the node, to be filled into
    /// @param  pTunedModule  Parsed tuned module
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID CopyTuningModeInfo(
        TuningMode*      pTuningMode,
        ParameterModule* pTunedModule);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsModeMatch
    ///
    /// @brief  Determine if a requested tuning mode for a module matches one of the tuned modes for the module
    ///
    /// @param  pRequest    Requested tuning mode
    /// @param  pTuned      One of the tuned modes of the module
    /// @param  pPrecedence Out parameter containing the precedence of the match (to be used if there is a tie)
    ///
    /// @return TRUE or FALSE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL IsModeMatch(
        TuningMode* pRequest,
        TuningMode* pTuned,
        UINT*       pPrecedence);

    // Do not implement the copy constructor or assignment operator
    TuningDataManager(const TuningDataManager& rTuningDataManager) = delete;
    TuningDataManager& operator= (const TuningDataManager& rTuningDataManager) = delete;

    TunedModulesInfo* m_pTunedModulesInfo;    ///< Stores the mode trees of all modules
    TunedBufferData*  m_pTunedBuffInfo;
    Mutex* mFileRead;
    BYTE*  m_pBuffer;         ///< Binary file
    BOOL   isChanged;
    BOOL   isModified;
    UINT64 mbufferLength;
};
CAMX_NAMESPACE_END

#endif // CAMXTUNINGDATAMANAGER_H
