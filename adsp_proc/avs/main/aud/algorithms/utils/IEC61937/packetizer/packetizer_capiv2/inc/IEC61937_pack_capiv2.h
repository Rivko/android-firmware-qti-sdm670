/* ======================================================================== */
/**
   @file iec61937_pack_capiv2.h

    Header file to implement common capiv2 interface for 61937 packetizers
*/

/* =========================================================================
Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
   ========================================================================== */

/* =========================================================================
   Edit History

     when       who          what, where, why
   --------   -------     -------------------------------------------------
   21/06/16   adeepak        Initial creation
   ======================================================================== */

#ifndef CAPIV2_IEC61937_PACK_H
#define CAPIV2_IEC61937_PACK_H

/*------------------------------------------------------------------------
 * Include files
 * -----------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"

/*------------------------------------------------------------------------
 * Function declarations
 * -----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

capi_v2_err_t capiv2_iec61937_pack_get_static_properties (capi_v2_proplist_t *init_set_properties, capi_v2_proplist_t *static_properties);

capi_v2_err_t capiv2_iec61937_pack_init (capi_v2_t *_pif, capi_v2_proplist_t *init_set_properties);

#ifdef __cplusplus
}
#endif

#endif //CAPIV2_IEC61937_PACK_H

