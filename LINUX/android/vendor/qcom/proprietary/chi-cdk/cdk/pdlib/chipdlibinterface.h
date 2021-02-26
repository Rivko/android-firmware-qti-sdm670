////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chipdlibinterface.h
/// @brief PD library interfaces
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// NOWHINE FILE GR016: Typedefs must not be used for structs, classes, and enumerations
// NOWHINE FILE NC009: File names must start with camx

#ifndef CHIPDLIBINTERFACE_H
#define CHIPDLIBINTERFACE_H

#include "camxcdktypes.h"
#include "chipdlibcommon.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/// @brief Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

/// @brief Represents  PD calibration data
typedef struct PDLibCreateParams
{
    PDLibInitParam  initParam;          ///< initialization parameter
    VOID*           pTuningDataManager; ///< Pointer to TuningDataManager object
} PDLibCreateParams;

/// @brief Identifies the type of config param
typedef enum
{
    PDConfigTypeInvalid         = -1,          ///< Invalid type
    PDConfigTypeIFEData,                       /// Payload: PDLibIFEData
    PDConfigTypeMax             = 0x7FFFFFFF
} PDConfigType;

/// @brief Represents PD lib configuration parameters
typedef struct
{
    PDConfigType         configType;    ///< type of config param
    PDLibDataBufferInfo  PDBufferInfo;  ///< ISP payload for Type 3.
} PDLibConfigParams;

/// @brief Identifies if the trigger is early or late type
typedef enum
{
    PDTriggerTypeInvalid = -1,   ///< Invalid type
    PDTriggerEarly,             ///< The trigger targeted for early processing
    PDTriggerNormal,            ///< The trigger targeted for normal processing
    PDTriggerTypeCount          ///< This will be used for array size
} PDTriggerType;

/// @brief Represents PD lib input parameters
typedef struct
{
    PDLibParam          processParam;           ///< Parameters to process PD raw buffer
    UINT                numOfWrittenPDlines;    ///< number of lines of PD Data written
    VOID*               pMemoryPool;            ///< Memory pointer to save PD output
    UINT64              requestId;              ///< current request id
    PDTriggerType       trigger;                ///< type of Trigger ie early or late
    VOID*               pChiStatsSession;       ///< stats chi session data
    StatisticsStartUp   startupMode;            ///< statistics start up mode.
} PDLibInputs;

/// @brief Represents PD lib output parameters
typedef struct
{
    PDLibOutputData libOutput;  ///< defocus data of main grids
} PDLibOutputs;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Interface for PD Library
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct CHIPDLib
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// PDLibInitialize
    ///
    /// @brief  This function pointer to method of initializing the CHIPDLib.
    ///
    /// @param  pPDLib          Pointer to the pointer of the created CHIPDLib instance
    /// @param  pCreateParam    Pointer to create para
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult(*PDLibInitialize)(
        CHIPDLib*                   pPDLib,
        const PDLibCreateParams*    pCreateParam);

    //////////////////////////////////////////////////////////////////////////////////////////////// ////////////////////////////
    /// PDLibDestroy
    ///
    /// @brief  This method destroys the derived instance of the interface
    ///
    /// @param  pPDLib          Pointer to CHIPDLib instance
    /// @param  pDestroyParams  Pointer to destroy Param List
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID(*PDLibDestroy)(
        CHIPDLib*                       pPDLib,
        const PDLibDestroyParamList*    pDestroyParams);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// PDLibConfigure
    ///
    /// @brief  Sets configuration parameters to CHIPDLib.
    ///
    /// @param  pPDLib      Pointer to CHIPDLib instance
    /// @param  pConfig     Configuration parameters to set to the Library.
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult(*PDLibConfigure)(
        CHIPDLib*                   pPDLib,
        const PDLibConfigParams*    pConfig);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// PDLibProcess
    ///
    /// @brief  Runs the pd library on the given inputs. This method must only be called after the library is
    ///         initialized.
    ///
    /// @param  pPDLib  Pointer to PD library instance
    /// @param  pInput  Pointer to stats inputs interface
    /// @param  pOutput Pointer to the stats data output by the algorithm
    ///
    /// @return CDKResultSuccess upon success.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CDKResult(*PDLibProcess)(
        CHIPDLib*           pPDLib,
        const PDLibInputs*  pInput,
        PDLibOutputs*       pOutput);

} CHIPDLib; // CHIPDLib interface

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CREATEPD
///
/// @brief  This method creates an instance to the CHIPDLib
///
/// @param  ppPDLib         Pointer to the created PD Library instance
/// @param  pCreateParams   Pointer to create params
///
/// @return CDKResultSuccess upon success.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult(*CREATEPD)(
    CHIPDLib**                  ppPDLib,
    const PDLibCreateParamList* pCreateParams);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHIPDLIBINTERFACE_H
