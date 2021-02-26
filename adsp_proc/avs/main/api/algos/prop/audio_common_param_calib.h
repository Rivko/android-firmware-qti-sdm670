#ifndef AUDIO_COMMON_PARAM_CALIB_H
#define AUDIO_COMMON_PARAM_CALIB_H

/*==============================================================================
  @file audio_common_param_calib.h
  @brief This file contains common parameters
==============================================================================*/

/*=======================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 05/21/17   shrani  Added h2xml comments.
 ========================================================================== */

#include "mmdefs.h"

/*==============================================================================
   Constants
==============================================================================*/
/**
    ID of the lib version parameter used by any audio processing module.

    This generic/common parameter is used to query the
    lib version of any audio processing module.

 */
#define AUDPROC_PARAM_ID_LIB_VERSION                                   0x00010937

/* Structure for Querying module lib version of any Audio processing modules. */
typedef struct audproc_lib_version_t audproc_lib_version_t;
/** @h2xmlp_parameter   {"AUDPROC_PARAM_ID_LIB_VERSION", AUDPROC_PARAM_ID_LIB_VERSION}
    @h2xmlp_description {To query the lib version of any audio processing module.}  
    @h2xmlp_toolPolicy  {RTC_READONLY}
    @h2xmlp_readOnly    {true}*/

#include "adsp_begin_pack.h"

/* Payload of the AUDPROC_PARAM_ID_LIB_VERSION parameter used by
 any Audio Processing module
 */
struct audproc_lib_version_t
{
   uint32_t lib_version_low;
   /**< @h2xmle_description  {Version of the module LSB.} */


   uint32_t lib_version_high;
    /**< @h2xmle_description  { Version of the module MSB} */

}
#include "adsp_end_pack.h"
;
#endif


