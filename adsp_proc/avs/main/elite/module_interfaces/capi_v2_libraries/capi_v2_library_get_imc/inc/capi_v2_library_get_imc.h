#ifndef CAPI_V2_LIBRARY_GET_IMC_H
#define CAPI_V2_LIBRARY_GET_IMC_H

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
1/21/15     sabdagir   Created file.
 ========================================================================= */

#include "Elite_lib_get_imc.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

capi_v2_err_t capi_v2_library_get_imc_create(elite_lib_get_imc_t **lib_ptr_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* #ifndef CAPI_V2_LIBRARY_GET_IMC_H */
