// NOWHINE ENTIRE FILE 
//-------------------------------------------------------------------------
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

#ifndef __PROCESS_ANR_PASS_H__
#define __PROCESS_ANR_PASS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "ANR_Chromatix.h"
#include "ANR_Registers.h"
#include "NcLibContext.h"



/**
*  @brief   Calculate ANR registers and FW structs for one frame.
*                                                 
*
*  @param [in]      chromatixHeaderPasses             Array of Chromatix structs for 1-4 passes, 
*                                                     the assumption is that the scales are [1:1, 1:4, 1:16, 1:64]
*  @param [in]      context                           ANR SW context struct
*  @param [out]     registerStructPasses              Array of registers structs for 1-4 passes. 
*                                                     the assumption is that the scales are [1:1, 1:4, 1:16, 1:64]
*  @param [out]     fwStruct                          The ANR FW struct that is filled by the function.
*  @param [in]      numOfPasses                       Number of passes to run
*  @param [in]      fullImageWidthPixels              ANR Image width (ICA1 output) in pixels
*  @param [in]      fullImageHeightPixels             ANR Image height (ICA1 output)in pixels
*  @param [in]      fullImageWidthPixelsWithMargins   ANR Image width (ICA1 output)in pixels with EIS margins.
*  @param [in]      fullImageHeightPixelsWithMargins  ANR Image height (ICA1 output) in pixels with EIS margins.
*  @param [in]      hwBitsNumber                      The number of bits that are supported by ANR HW.
*                                                     Right now only 10 is supported.
*  @return NC_LIB_SUCCESS in case of success, otherwise failed.
*/
int32_t ANR_FlowProcessNcLib(
    ANR_Chromatix* chromatixHeaderPasses,
    NCLIB_CONTEXT_ANR* context,
    ANR_REG_PASS* registerStructPasses,
    AnrParameters* fwStruct,
    uint32_t numOfPasses,
    uint32_t fullImageWidthPixels,
    uint32_t fullImageHeightPixels,
    uint32_t fullImageWidthPixelsWithMargins,
    uint32_t fullImageHeightPixelsWithMargins,
    uint32_t hwBitsNumber);

/**
*  @brief   Set default values for ANR FW struct
*
*  @param [in]      fwStruct               The FW struct that is set.
*
*  @return void
*/
void SetDefaultsForAnrStruct(AnrParameters* fwStruct);

/**
*  @brief   Validate ANR SW Context
*
*  @param [in]      nclibContext           ANR SW context struct to validate
*  @param [in]      fullImageWidthPixels   ANR Full image width in pixels
*  @param [in]      fullImageHeightPixels  ANR Full image height in pixels
*
*  @return NC_LIB_SUCCESS in case of success, otherwise failed.
*/
int32_t ValidateAnrContext(
    NCLIB_CONTEXT_ANR* nclibContext,
    uint32_t fullImageWidthPixels,
    uint32_t fullImageHeightPixels);

/**
*  @brief   Generates assist grid from matrices
*
*  @param [in]      registerStruct      Register struct to validate
*  @param [in]      numOfPasses         Number of passes to validate.
*                                       If set to 1, FULL pass is validated.
*                                       If set to 2, FULL and DC4 passes are validated.
*                                       If set to 3, FULL, DC4, and DC16 passes are validated.
*                                       If set to 4, FULL,DC4, DC16 and DC64 passes are validated.
*  @param [in]      mE                  The number of bits that are supported by ANR HW (hwBitsNumber) 
*                                       is defined in the formula (8+mE). Right now only mE == 2 is supported.
*
*  @return NC_LIB_SUCCESS in case of success, otherwise failed.
*/
int32_t ANR_ValidateNcLibRegs(ANR_REG_PASS* registerStruct, uint32_t numOfPasses, uint32_t mE);

/**
*  @brief   Check whether noise reduction is enabled in Chromatix for specific pass
*
*  @param [in]      chromatix           ANR chromatix structure for the tested pass
*  @param [out]     yEnabled            Return value for Luma channel
*  @param [out]     cEnabled            Return value for Chroma channel
*
*  @return void
*/
void ANR_IsPassEnabled(ANR_Chromatix* chromatix, NCLIB_BOOLEAN* yEnabled, NCLIB_BOOLEAN* cEnabled);

#ifdef __cplusplus
}
#endif


#endif //__PROCESS_ANR_PASS_H__

