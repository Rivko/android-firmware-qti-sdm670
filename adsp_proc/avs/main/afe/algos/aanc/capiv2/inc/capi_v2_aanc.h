/**
@file capi_v2_aanc.h

@brief CAPI V2 API wrapper for AANC lib

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/algos/aanc/capiv2/inc/capi_v2_aanc.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
4/4/2014   rv       Created
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/

#ifndef CAPI_V2_AANC_H_
#define CAPI_V2_AANC_H_


#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"
#include "adsp_error_codes.h"
#include "mmdefs.h"

/*----------------------------------------------------------------------------
 * Macro definitions
 * -------------------------------------------------------------------------*/
 
/** Macro for custom param id from CAPI V2 modules */
#define CAPI_V2_AFE_AANC_PARAM_ID_GAIN_1            0x00001001
#define CAPI_V2_AFE_AANC_PARAM_ID_GAIN_2            0x00001002
#define CAPI_V2_AFE_AANC_PARAM_ID_LIB_SIZE          0x00001003
#define CAPI_V2_AFE_AANC_PARAM_ID_LIB_PTR           0x00001004
#define CAPI_V2_AFE_AANC_PARAM_ID_LIB_SAMPLING_RATE 0x00001005
#define CAPI_V2_AFE_AANC_PARAM_ID_ACTUAL_FRAME_SIZE 0x00001006
#define CAPI_V2_AFE_AANC_PARAM_ID_LIB_ALL_CFG_RCVD  0x00001009
#define CAPI_V2_AFE_AANC_PARAM_ID_LIB_VALID_CFG     0x00001010


/** Macro for custom property id from CAPI V2 modules */
#define CAPI_V2_AFE_AANC_PROP_ID_INIT_FRAME_BUFFER_TX 0x00002001
#define CAPI_V2_AFE_AANC_PROP_ID_INIT_FRAME_BUFFER_RX 0x00002002
#define CAPI_V2_AFE_AANC_PROP_ID_INIT_LIB_CFG         0x00002003
#define CAPI_V2_AFE_AANC_PROP_ID_INIT_LOG             0x00002004

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

capi_v2_err_t capi_v2_aanc_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_aanc_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* CAPI_V2_AANC_H_ */
