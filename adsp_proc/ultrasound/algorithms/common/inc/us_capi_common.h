#ifndef US_CAPI_COMMON_H
#define US_CAPI_COMMON_H

/* ========================================================================
  *//** @file us_capi_common.h
  Definitions common to all Ultrasound CAPI libraries

  Copyright (c) 2013 - 2014 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/* =========================================================================
                             Edit History

   when       who            what, where, why
   --------   --------     -- ----------------------------------------------
   06/10/14   HS           Added interface class for the ultrasound CAPI libraries
   11/26/13   LD           Initial version
   ========================================================================= */


/* =======================================================================
 *                       DEFINITIONS AND DECLARATIONS
 * ====================================================================== */
#include <Elite_CAPI.h>
#include "UltrasoundStreamMgr_Util.h"

/**
 * Common parameters for all Ultrasound CAPI encoders
 */
enum UsCapiEncoderParamIdx
{
    eUsCapiEncoderHasLeftOverFrames = eIcapiMaxParamIndex,
    /**< Flag if the encoder gathered enough left over data for an additional 
         full frame or more. */
    eUsCapiEncoderCommonMaxParamIndex = eIcapiMaxParamIndex + 0x100
    /**< Maximum value or common encoder parameter, can be used as first 
         index of encoder-specific parameter */
};

class IUsCAPI : public ICAPI
{
public:
    virtual int SetGetLibParam(param_data_t* pParamData)
    {
        return ADSP_EOK;
    }
};

#endif /* US_ALGORITHMS_COMMON_H */

