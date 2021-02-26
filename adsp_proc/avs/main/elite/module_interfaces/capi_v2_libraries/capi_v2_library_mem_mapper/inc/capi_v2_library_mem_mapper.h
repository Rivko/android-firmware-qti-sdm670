#ifndef CAPI_V2_LIBRARY_GET_MEM_MAPPER_H
#define CAPI_V2_LIBRARY_GET_MEM_MAPPER_H

/* ========================================================================*/
/**
 @file capi_v2_library_get_mem_mapper.h
 Header file for a library that allows querying for modules from the AMDB.
 */

/*======================================================================
 Copyright (c) 2017 QUALCOMM Technologies Incorporated.
 All rights reserved. Qualcomm Proprietary and Confidential.
 ====================================================================== */

/* =========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     ------------------------------------------------------
 2/11/2017  rbhatnk      Created file.
 ========================================================================= */

#include "Elite_lib_mem_mapper.h"

capi_v2_err_t capi_v2_lib_mem_mapper_create(elite_lib_mem_mapper_t **lib_ptr_ptr);

#endif /* #ifndef CAPI_V2_LIBRARY_GET_MEM_MAPPER_H */
