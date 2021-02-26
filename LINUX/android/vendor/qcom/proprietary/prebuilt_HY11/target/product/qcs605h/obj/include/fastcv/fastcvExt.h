#ifndef FASTCVEXT_H
#define FASTCVEXT_H

/**=============================================================================

@file
   fastcvExt.h

@brief
   Public extended API

Copyright (c) 2014, 2017 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

=============================================================================**/

//==============================================================================
// Included modules
//==============================================================================
#include "fastcv.h"

#ifdef __cplusplus
extern "C"
{
#endif

//==============================================================================
// Declarations
//==============================================================================

//------------------------------------------------------------------------------
/// @brief
///    Defines operational mode of interface to allow the end developer to
///    dictate how the target optimized implementation should behave.
//------------------------------------------------------------------------------
typedef enum
{
   /// Target-optimized implementation uses as much QDSP Unit as possible
   /// For general DSP offloading, FASTCV_OP_EXT_QDSP is recommended
   /// For mobile targets, use FASTCV_OP_EXT_QDSP option
   FASTCV_OP_EXT_QDSP,

   /// Target-optimized implementation uses as much Graphics Processing Unit as possible
   FASTCV_OP_EXT_GPU,

   /// Target-optimized implementation uses as much ADSP Unit as possible on supported targets
   /// This mode is supported on IOT target only
   FASTCV_OP_EXT_ADSP,

   /// Target-optimized implementation uses as much CDSP Unit as possible on supported targets
   /// This mode is supported on IOT target only
   FASTCV_OP_EXT_CDSP,

   /// Target-optimized implementation uses as much MDSP Unit as possible on supported targets
   /// This mode is supported on IOT target only
   FASTCV_OP_EXT_MDSP

} fcvOperationModeExt;

//---------------------------------------------------------------------------
/// @brief
///   Selects HW units for all routines at run-time.  Can be called anytime to
///   update choice.
///
/// @param mode
///   See enum for details.
///
/// @return
///   FASTCV_SUCCESS if successful.
///   FASTCV_EFAIL if unsuccessful.
///
/// @ingroup misc
//---------------------------------------------------------------------------

FASTCV_API fcvStatus
fcvSetOperationModeExt( fcvOperationModeExt modeExt );

#ifdef __cplusplus
}//extern "C"
#endif

#endif
