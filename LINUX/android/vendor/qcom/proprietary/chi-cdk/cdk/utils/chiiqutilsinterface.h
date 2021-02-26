////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chiiqutilsinterface.h
/// @brief Chi iq utility interface definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NOWHINE FILE GR016: Typedefs must not be used for structs, classes, and enumerations

#ifndef CHIIQUTILSINTERFACE_H
#define CHIIQUTILSINTERFACE_H

#include "camxcdktypes.h"
#include "camxchinodeutil.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

/// Rules of input/output/param structures and enum
/// 1. Every new variable has to be added to the end of the structure and enum once the interface is published
/// 2. No change to the sub-structures inside of the structures once published, if has to be changed, add the new variables to
///    the end of the master structure and enum
///

/// @brief Defines Chi IQ utility
typedef enum ChiIQUtilsParamsType
{
    IQUtilsInvalid                            = -1,          ///< The invalid type
    IQUtilsCalculateInverseGrid               = 0,           ///< IQ utils type to calculate inverse grid
    IQUtilsMax                                = 0x7FFFFFFF   ///< Anchor to indicate the last item in the defines
} CHIIQUTILSPARAMSTYPE;

/// @brief Defines Chi IQ utility parameters
typedef struct ChiIQUtilsParams
{
    CHIIQUTILSPARAMSTYPE     paramType;                      ///< Chi IQ Utils param type
    VOID*                    pInputData;                     ///< Pointer to input data
    VOID*                    pOutputData;                    ///< Pointer to output data
} CHIIQUTILSPARAMS;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROCESSCHIIQUTILS
///
/// @brief  used for calling the Chi IQ utility by the external node.
///
///         Function pointer defines the interface for external node to use the required Chi IQ utility on initializing
///         the interface. Any variables stored in the node must be protected against multiple sessions accessing it at
///         the same time.
///
/// @param  [in,out] pChiIQUtilsParams  Pointer to a structure that defines chi iq utils parameters that the node provides
///                                     to the interface. The node must fill in these function pointers.
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PROCESSCHIIQUTILS) (CHIIQUTILSPARAMS* pChiIQUtilsParams);

/// @brief Defines Chi IQ utility interface
typedef struct ChiIQUtils
{
    PROCESSCHIIQUTILS pProcessChiIQUtils;
} CHIIQUTILS;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CHIIQUTILSGET
///
/// @brief  used for initializing the Chi IQ utility interface for the external node.
///
///         Function pointer for external node to initialize the Chi IQ utility interface. In addition to communicating the
///         necessary function pointers between IQ utility framework and external nodes, this function allows a node to do
///         any initialization. Any variables stored in the node must be protected against multiple sessions accessing it at
///         the same time.
///
/// @param  [in,out] pChiIQUtils  Pointer to a structure that defines chi iq utils handler that the Chi IQ interface sends to
///                               the node. The node must fill in these function pointers.
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (*CHIIQUTILSGET) (CHIIQUTILS* pChiIQUtils);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ChiIQUtilsGet
///
/// @brief  used for initializing the Chi IQ utility interface for the external node.
///
///         This function is exported by <library>.so in order for external node to initialize the Chi IQ utility interface.
///         In addition to communicating the necessary function pointers between IQ utility framework and external nodes,
///         this function allows a node to do any initialization. Any variables stored in the node must be protected against
///         multiple sessions accessing it at the same time.
///
/// @param  [in,out] pChiIQUtils  Pointer to a structure that defines chi iq utils handler that the Chi IQ interface sends to
///                               the node. The node must fill in these function pointers.
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDK_VISIBILITY_PUBLIC VOID ChiIQUtilsGet(
    CHIIQUTILS* pChiIQUtils);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHIIQUTILSINTERFACE_H
