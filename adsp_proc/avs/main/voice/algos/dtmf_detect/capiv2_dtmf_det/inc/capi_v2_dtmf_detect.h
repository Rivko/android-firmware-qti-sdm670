#ifndef CAPI_V2_DTMF_DETECT_H_
#define CAPI_V2_DTMF_DETECT_H_


/*========================================================================
Edit History

when        who      what, where, why
--------    ---      -------------------------------------------------------
05/23/2016	aabraham Initial version
==========================================================================*/

/*------------------------------------------------------------------------
   Copyright (c) 2017 Qualcomm Technologies Inc. All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential
-------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
 *   Include Files
 *-----------------------------------------------------------------------*/


#include "Elite_CAPI_V2.h"

#if defined(__cplusplus)
extern "C" {
#endif    //__cplusplus

/*----------------------------------------------------------------------------
 * Function declarations 
 *----------------------------------------------------------------------------*/
 

/* Functions for voice tx dtmf interface*/
 capi_v2_err_t capi_v2_voice_tx_dtmf_det_get_static_properties (
    capi_v2_proplist_t *init_set_properties,
    capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_voice_tx_dtmf_det_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


/* Functions for voice rx dtmf interface */
capi_v2_err_t capi_v2_voice_dtmf_det_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);   

capi_v2_err_t capi_v2_voice_dtmf_det_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


/* Functions for audio interface */
capi_v2_err_t capi_v2_aud_dtmf_det_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

capi_v2_err_t capi_v2_aud_dtmf_det_init (
	capi_v2_t      		*_pif,
	capi_v2_proplist_t		*init_set_properties);

 
#if defined (__cplusplus) 
}
#endif // __cplusplus
 

#endif /* CAPI_V2_DTMF_DETECT_H_ */
