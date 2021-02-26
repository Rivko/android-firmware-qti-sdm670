/*==========================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
* =========================================================================*/

/* 
* @file capi_v2_lec.h
*
* capiv2 interface for lec 
*/

/*==========================================================================
* Edit History :
* when                     who                      what,where,why
*---------------        ----------------         -----------------------
* 25/02/2016             aabraham	             capiv2 entry point interface 
* =========================================================================*/


#ifndef CAPI_V2_LEC_H
#define CAPI_V2_LEC_H


#include "Elite_CAPI_V2.h"
#include "mmdefs.h"

#ifdef __cplusplus
	extern "C" {
#endif

/*
*  Get static properies of lec module 
*/
capi_v2_err_t capi_v2_lec_get_static_properties(
	capi_v2_proplist_t *init_set_properties,
	capi_v2_proplist_t *static_properties);

/*
 *  Instantiate the module memory 
 */

capi_v2_err_t capi_v2_lec_init(
	capi_v2_t             *_pif,
	capi_v2_proplist_t    *init_set_properties);

#ifdef __cplusplus
	}
#endif


#endif /* CAPI_V2_LEC_H */