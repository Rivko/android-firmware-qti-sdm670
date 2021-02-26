/**
@file capi_v2_voice_resampler.h

@brief CAPI V2 API wrapper for voice resampler algorithm

*/

/*========================================================================

Edit History

when       who            what, where, why
--------   ---             -------------------------------------------------------
4/1/2015  sabdagir       Created
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef CAPI_V2_VOICE_RESAMPLER
#define CAPI_V2_VOICE_RESAMPLER

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"

/*------------------------------------------------------------------------
 * Macros, Defines, Type declarations
 * -----------------------------------------------------------------------*/

/* Set param for sending input frame size prior to sending INPUT_MEDIA_FMT 
   IIR Resampler will allocate channel memory and configure the algorithm
   with this input frame size*/ 
#define CAPI_V2_IIR_RESAMPLER_INPUT_FRAME_SIZE 0x1001
/* Data structure used when sending input frame size to iir resampler.
*/
typedef struct 
{
  uint32_t input_frame_size_us;
  /* input frame size in micro seconds*/

}input_frame_size_t; 
/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

capi_v2_err_t capi_v2_voice_resampler_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_voice_resampler_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // CAPI_V2_VOICE_RESAMPLER
