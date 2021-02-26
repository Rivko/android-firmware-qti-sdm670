// NOWHINE ENTIRE FILE 
//-------------------------------------------------------------------------
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

#ifndef __PROCESS_TF_H__
#define __PROCESS_TF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "TF_Chromatix.h"
#include "TF_Registers.h"
#include "NcLibContext.h"

/*------------------------------------------------------------------------
*       API Functions
* ----------------------------------------------------------------------- */

/**
*  @brief   Function that helps to set frame level registers (non geometric) according to flow settings. 
*           There is assumption that this function is called after TF_ProcessNcLib
*
*
*  @param [in]      chromatixStruct           TF Chromatix struct
*  @param [in]      context                   Contains SW information which is relevant for activation
*  @param [in]      curPassScalingRatioLog4   Define which pass is used
*                                             0 - full pass
*                                             1 - DC4  pass
*                                             2 - DC16 pass
*                                             3 - DC64 pass
*  @param [in]      numOfPasses               Defines how many passes are used
*  @param [in,out]  regStruct                 Output TF register struct
*  @param [in,out]  fwStructRefinement        Output Refinement FW struct
*  @param [in,out]  fwStructTf                Output TF FW struct
*
*  @return NC_LIB_SUCCESS in case of success, otherwise failed.
*/
int32_t TF_ConcludeFrameLevelRegistersConfiguration(
    TF_Chromatix* chromatixStruct,
    NCLIB_CONTEXT_TF* context, 
    uint32_t curPassScalingRatioLog4,
    uint32_t numOfPasses,
    TF_REG* regStruct, 
    RefinementPassParameters* fwStructRefinement,
    TfPassParameters* fwStructTf);

/**
*  @brief   Function that help to convert Chromatix settings into register and FW structs
*
*
*  @param [in]      chromatixStruct           TF Chromatix struct
*  @param [in]      flow                      Define in which flow mode TF is activated
*  @param [in]      curPassScalingRatioLog4   Define which pass is used
*                                             0 - full pass
*                                             1 - DC4  pass
*                                             2 - DC16 pass
*                                             3 - DC64 pass
*  @param [in,out]  regStruct                 Output TF register struct
*  @param [in,out]  fwStructTf                Output TF FW struct
*  @param [out]     fwStructRefinement        Output Refinement FW struct
*
*  @return NC_LIB_SUCCESS in case of success, otherwise failed.
*/
int32_t TF_ProcessNcLib(
    TF_Chromatix* chromatixStruct,
    EIPEConfigOption flow,
    uint32_t curPassScalingRatioLog4,
    TF_REG* regStruct,
    TfPassParameters* fwStructTf,
    RefinementPassParameters* fwStructRefinement);


/**
*  @brief   Function that calculate some parameters which are related to warp results (e.g. update
*           geometric registers and registers which are related to LRME results).
*           There is assumption that this function is called after TF_ProcessNcLib and 
*           TF_ConcludeFrameLevelRegistersConfiguration
*
*
*  @param [in]      chromatixStruct           TF Chromatix struct
*  @param [in]      context                   Contains SW information which is relevant for activation
*  @param [in]      curPassScalingRatioLog4   Define which pass is used
*                                             0 - full pass
*                                             1 - DC4  pass
*                                             2 - DC16 pass
*                                             3 - DC64 pass
*  @param [in]      transformConfidenceVal    Defines mapping function from calculated transform 
*                                             confidence to actually used transform confidence.
*                                             The calculated confidence is in the 
*                                             range 0:256 (8 bit fraction). format: 9uQ8
*                                             In case transformConfidenceVal is set to 256, the 
*                                             confidence has no effect 
*  @param [in,out]  regStruct                 Output TF register struct
*  @param [in,out]  fwStructTf                Output TF FW struct
*
*  @return NC_LIB_SUCCESS in case of success, otherwise failed.
*/
int32_t TF_CalcWarpDependedParams(
    TF_Chromatix* chromatixStruct,
    NCLIB_CONTEXT_TF* context,
    uint32_t curPassScalingRatioLog4, 
    uint32_t transformConfidenceVal,
    TF_REG* regStruct,
    TfPassParameters* fwStructTf);


/**
*  @brief   Validate TF SW Context
*
*
*  @param [in]      nclibContext              Contains SW information which is relevant for activation
*  @param [in]      curPassScalingRatioLog4   Define which pass is used
*                                             0 - full pass
*                                             1 - DC4  pass
*                                             2 - DC16 pass
*                                             3 - DC64 pass
*
*  @return NC_LIB_SUCCESS in case of success, otherwise failed.
*/
int32_t ValidateTfContext(NCLIB_CONTEXT_TF* nclibContext, uint32_t curPassScalingRatioLog4);

/**
*  @brief   Validate TF Refinement calculation result
*
*
*  @param [in]      fwStructTf                TF FW struct
*  @param [in]      fwStructRefinement        Refinement FW struct
*  @param [in]      curPassScalingRatioLog4   Define which pass is used
*                                             0 - full pass
*                                             1 - DC4  pass
*                                             2 - DC16 pass
*                                             3 - DC64 pass
*
*  @return NC_LIB_SUCCESS in case of success, otherwise failed.
*/
int32_t ValidateTfCalc(TfPassParameters* fwStructTf, RefinementPassParameters* fwStructRefinement, uint32_t curPassScalingRatioLog4);

/**
*  @brief   Set default FW structs settings
*           Should be called immediately after TfPassParameters and RefinementPassParameters creation
*
*
*  @param [in]      fwStructTf                TF FW struct
*  @param [in]      fwStructRefinement        Refinement FW struct
*/
void SetDefaultsForTFStructs(TfPassParameters* fwStructTf, RefinementPassParameters* fwStructRefinement);


/**
*  @brief   Function that validates that relationship between different Chromatix settings are valid.
*
*
*  @param [in]      chromatixStruct           TF Chromatix struct
*  @param [in]      nclibContext              Contains SW information which is relevant for activation
*  @param [in]      curPassScalingRatioLog4   Define which pass is used
*                                             0 - full pass
*                                             1 - DC4  pass
*                                             2 - DC16 pass
*                                             3 - DC64 pass
*
*  @return NC_LIB_SUCCESS in case of success, otherwise failed.
*/
int32_t Validate_TF_ChromatixParamRelations(
    TF_Chromatix* chromatixStruct,
    NCLIB_CONTEXT_TF* nclibContext,
    uint32_t curPassScalingRatioLog4);


#ifdef __cplusplus
}
#endif


#endif //__PROCESS_TF_H__

