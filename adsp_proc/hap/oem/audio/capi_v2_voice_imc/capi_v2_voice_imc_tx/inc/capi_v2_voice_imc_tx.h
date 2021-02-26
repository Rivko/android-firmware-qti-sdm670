/*-----------------------------------------------------------------------
   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/

#ifndef CAPI_V2_VOICE_IMC_TX_H
#define CAPI_V2_VOICE_IMC_TX_H


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"

// Global IDs used for inter module communication
#define IMC_COMM_ID  0x10012D16

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

// IMC TX get_static_properties function declaration
capi_v2_err_t capi_v2_voice_imc_tx_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);

// IMC TX init function
capi_v2_err_t capi_v2_voice_imc_tx_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


#ifdef __cplusplus
}
#endif //__cplusplus
#endif //CAPI_V2_VOICE_IMC_TX_H
