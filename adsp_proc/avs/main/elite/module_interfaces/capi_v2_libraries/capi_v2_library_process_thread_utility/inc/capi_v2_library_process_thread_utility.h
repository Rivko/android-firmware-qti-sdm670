#ifndef _CAPI_V2_PROCESS_THREAD_UTIL_H
#define _CAPI_V2_PROCESS_THREAD_UTIL_H

/* ========================================================================*/
/**
  @file capi_v2_library_process_thread_utility.h

  Header file for a library to provide a separate processing thread for
  CAPIv2 modules that process in fixed sized frames.
  */

/*======================================================================
  Copyright (c) 2015 QUALCOMM Technologies Incorporated.
  All rights reserved. Qualcomm Proprietary and Confidential.
  ====================================================================== */

/* =========================================================================
   Edit History

   when       who     what, where, why
   --------   ---     ------------------------------------------------------
   05/08/15   bvodapal  Created file.
   ========================================================================= */

#include "Elite_lib_capi_v2_process_thread.h"

capi_v2_err_t capi_v2_library_process_thread_utils_create(elite_lib_capi_v2_process_thread_util_t **lib_ptr_ptr);


#endif /* _CAPI_V2_PROCESS_THREAD_UTIL_H */



