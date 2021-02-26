#ifndef CAPI_V2_LIBRARY_GET_MODULE_H
#define CAPI_V2_LIBRARY_GET_MODULE_H

/* ========================================================================*/
/**
 @file capi_v2_library_get_module.h
 Header file for a library that allows querying for modules from the AMDB.
 */

/*======================================================================
 Copyright (c) 2014 QUALCOMM Technologies Incorporated.
 All rights reserved. Qualcomm Proprietary and Confidential.
 ====================================================================== */

/* =========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     ------------------------------------------------------
 12/31/14   dg      Created file.
 ========================================================================= */

#include "Elite_lib_get_capi_v2_module.h"

capi_v2_err_t capi_v2_library_get_module_create(elite_lib_get_capi_v2_t **lib_ptr_ptr);

#endif /* #ifndef CAPI_V2_LIBRARY_GET_MODULE_H */
