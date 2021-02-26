// NOWHINE ENTIRE FILE
//-------------------------------------------------------------------------
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

#ifndef __PROCESS_ICA_H__
#define __PROCESS_ICA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ICA_Chromatix.h"
#include "ICA_Registers.h"
#include "NcLibContext.h"
#include "NcLibWarpCommonDef.h"

/**
*  @brief   Calculate ICA registers and FW struct which are set directly by Chromatix.
*
*  @param [in]      chromatixStruct             pointer to the pass Chromatix struct
*  @param [in]      icaIsGridEnabledByFlow      indication if Grid is set by Chromatix or SW.
*                                               if it is set to 0, it is taken from Chromatix
*                                               else it is set by SW (ICA_ProcessNonChromatixParams).
*  @param [out]     regStruct                   pointer to the pass register struct
*  @param [out]     fwStruct                    FW struct that is filled by the function.
*
*  @return NC_LIB_SUCCESS in case of success, otherwise failed.
*/
int32_t ICA_ProcessNcLib(
    const ICA_Chromatix* chromatixStruct,
    uint8_t icaIsGridEnabledByFlow,
    ICA_REG* regStruct,
    IcaParameters* fwStruct);

/**
*  @brief   Calculate ICA non Chromatix registers and FW structs .
*
*  @param [in]      inGridFlow                  pointer to grid.
*                                               if chip is Napali:
*                                                   inGridFlow has to be grid of exact size of ICA v10 : 
*                                                   ICA_GRID_TRANSFORM_WIDTH_V10 = 33, ICA_V10_GRID_TRANSFORM_HEIGHT = 25
*                                               if chip is Hana:
*                                                   inGridFlow has to be grid of exact size of ICA v20 :
*                                                   ICA_V20_GRID_TRANSFORM_WIDTH = 35, ICA_V20_GRID_TRANSFORM_HEIGHT = 27
*  @param [in]      persp                       pointer to perspective transform matrices.
*                                               There are up to be max 9 matrices.
*  @param [in]      icaIsGridEnabledByFlow      indication if Grid is set by Chromatix or SW.
*                                               if it is set to 0, it is taken from Chromatix (ICA_ProcessNcLib)
*                                               else it is set by SW (this function).
*  @param [in]      icaIsRefinementEnabled      indication if Refinement is enabled.
*  @param [out]     regStruct                   pointer to the pass register struct
*  @param [out]     fwStruct                    FW struct that is filled by the function.
*
*  @return NC_LIB_SUCCESS in case of success, otherwise failed.
*/
int32_t ICA_ProcessNonChromatixParams(
    const NcLibWarpGrid* inGridFlow,
    const NcLibWarpMatrices* persp,
    uint8_t icaIsGridEnabledByFlow,
    uint32_t icaIsRefinementEnabled,
    ICA_REG* regStruct,
    IcaParameters* fwStruct);

/**
*  @brief   Gets the inputs of ICA_ProcessNonChromatixParams and return true or false if they are valid for ICA
*
*  @param [in]      inGridFlow                  pointer to grid.
*                                               if chip is Napali:
*                                                   inGridFlow has to be grid of exact size of ICA v10 :
*                                                   ICA_GRID_TRANSFORM_WIDTH_V10 = 33, ICA_V10_GRID_TRANSFORM_HEIGHT = 25
*                                               if chip is Hana:
*                                                   inGridFlow has to be grid of exact size of ICA v20 :
*                                                   ICA_V20_GRID_TRANSFORM_WIDTH = 35, ICA_V20_GRID_TRANSFORM_HEIGHT = 27
*  @param [in]      persp                       pointer to perspective transform matrices.
*                                               There are up to be max 9 matrices.
*  @param [in]      icaIsGridEnabledByFlow      indication if Grid is set by Chromatix or SW.
*                                               if it is set to 0, it is taken from Chromatix (ICA_ProcessNcLib)
*                                               else it is set by SW (this function).
*  @param [in]      icaIsRefinementEnabled      indication if Refinement is enabled.
*
*  @return NC_LIB_SUCCESS in case of success, otherwise failed.
*/
int32_t ValidateIcaNonChromatixParams(
    const NcLibWarpGrid* inGridFlow,
    const NcLibWarpMatrices* persp,
    uint8_t  icaIsGridEnabledByFlow,
    uint32_t icaIsRefinementEnabled);


/**
*  @brief   Set default values for ICA FW struct
*
*  @param [in]      fwStruct               The FW struct that is set.
*
*  @return void
*/
void SetDefaultsForIcaStruct(IcaParameters* fwStruct);

#ifdef __cplusplus
}
#endif


#endif //__PROCESS_ICA_H__

