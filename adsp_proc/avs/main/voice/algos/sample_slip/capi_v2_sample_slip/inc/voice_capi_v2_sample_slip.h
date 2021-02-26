/*-----------------------------------------------------------------------
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 -----------------------------------------------------------------------*/

#ifndef VOICE_CAPI_V2_SAMPLE_SLIP_H_
#define VOICE_CAPI_V2_SAMPLE_SLIP_H_

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"


#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus



/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

capi_v2_err_t sample_slip_capi_v2_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t sample_slip_capi_v2_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


typedef enum
{
   SS_MODE_1 = 1, /* EC present but does not support rate matching */
   SS_MODE_2,     /* EC present and supports rate matching  */
   SS_MODE_3,     /* Force Async mode when EC is present */
   SS_MODE_4,     /* No EC present in Tx topology */
   SS_MODE_5,     /* Used for Rx path (data sent to AFE). Drift in this code is corrected from VFR to codec rate  */
}ss_operating_modes_t;

typedef struct set_ss_operating_mode_t
{
   int32_t  operating_mode;
}set_ss_operating_mode_t;

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* VOICE_CAPI_V2_SAMPLE_SLIP_H_ */
