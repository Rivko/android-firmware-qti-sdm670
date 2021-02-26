/* ======================================================================== */
/**
    @file CAPI_V2_LIMITER24.h

    Header file to implement the Audio/Voice Post Processor Interface for
    MBDRC module.
*/

/* =========================================================================
Copyright (c) 2015-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
   ========================================================================= */

/* =========================================================================
   Edit History

     when       who          what, where, why
   --------   -------     -------------------------------------------------
   10/15/14     kn          Initial creation
   03/28/14   hbansal       Updated for audio interface.
   ======================================================================== */

#ifndef CAPI_V2_LIMITER24
#define CAPI_V2_LIMITER24

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"

/* Set param for sending bypass mode info to library*/
#define CAPI_V2_LIMITER_BYPASS_MODE 0x1001

/* Set param for sending config params to library*/
#define CAPI_V2_LIMITER_PARAM_ID_CONFIG_PARAMS 0x1002

/* Data structure used when sending bypass mode info.
*/
typedef struct
{
  uint32_t bypass_mode;
  /* bypass mode info*/
}bypass_mode_t;
typedef struct
{
  uint16_t Version;
  int16_t sLimMode;
  int16_t sLimMakeUpGain;
  int16_t sLimGc;
  int16_t sLimDelay;
  int16_t sLimMaxWait;
  int32_t nLimThreshold;
} tx_stream_limiter_t;

/* 
Data structure used when sending config params info.
*/
typedef struct
{
  uint32_t version; 
  
  uint32_t mode;        // Limiter processing mode
  uint32_t delay;       //Limiter delay and waiting time
  uint32_t history_window_length;/* Limiter peak history window length
                   When zero, zero-crossing processing is enabled. 
                   When nonzero, peak history processing is enabled. */  
  uint32_t threshold;    //Limiter threshold
  uint32_t makeup_gain;  //Limiter makeuo gain
  uint32_t gc;           //Limiter Gain recovery constant
  uint32_t max_wait;     //Limiter max wait time
  uint32_t gain_attack;  //Limiter gain attack constant
  uint32_t gain_release; //Limiter gain release constant
  uint32_t attack_coef;  //Limiter gain attack time speed coef
  uint32_t release_coef; //Limiter release attack time speed coef
  uint32_t hard_threshold; //Threshold for hard limiter
}limiter_config_params_t;

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/
capi_v2_err_t capi_v2_limiter24bit_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_limiter24bit_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // CAPI_V2_LIMITER24
