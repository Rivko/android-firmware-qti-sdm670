#ifndef CAPI_V2_LIBRARY_FACTORY_H
#define CAPI_V2_LIBRARY_FACTORY_H

/* ========================================================================*/
/**
 @file capi_v2_library_factory.h
 Header file for a factory utility to create instances of CAPIv2 supporting
 libraries.
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

#include "Elite_CAPI_V2_types.h"

capi_v2_err_t capi_v2_library_factory_get_instance(uint32_t id, void **lib_ptr);

#endif /* #ifndef CAPI_V2_LIBRARY_FACTORY_H */
