/*==========================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
* =========================================================================*/

/*
* @file capi_v2_mec.h
*
* capiv2 interface for mec
*/

/*==========================================================================
* Edit History :
* when                     who                      what,where,why
*---------------        ----------------         -----------------------
* 25/07/2016             sabdagir	             capiv2 entry point interface
* =========================================================================*/


#ifndef CAPI_V2_MEC_H
#define CAPI_V2_MEC_H


#include "Elite_CAPI_V2.h"

#ifdef __cplusplus
	extern "C" {
#endif

/*
*  Get static properties of mec module
*/
capi_v2_err_t __attribute__ ((visibility ("default")))  capi_v2_mec_get_static_properties(
	capi_v2_proplist_t *init_set_properties,
	capi_v2_proplist_t *static_properties);

/*
 *  Instantiate the module memory
 */

capi_v2_err_t __attribute__ ((visibility ("default")))  capi_v2_mec_init(
	capi_v2_t             *_pif,
	capi_v2_proplist_t    *init_set_properties);

#ifdef __cplusplus
	}
#endif


#endif /* CAPI_V2_MEC_H */
