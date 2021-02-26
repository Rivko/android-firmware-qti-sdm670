/*========================================================================
ADSP AMDB Parallel Loader

This file describes the interface for the parallel loader for AMDB. This utility
calls dlopen for multiple modules concurrently.

Copyright (c) 2016 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Technologies Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/13/15   DG      Created file.

========================================================================== */

#ifndef ADSP_AMDB_PARALLEL_LOADER_H
#define ADSP_AMDB_PARALLEL_LOADER_H


/*----------------------------------------------------------------------------
 * Include files
 * -------------------------------------------------------------------------*/
#ifndef ADSP_AMDB_TEST
#include "adsp_amdb_static.h"
#else
#include "test_main.h"
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

struct amdb_parallel_loader;
struct amdb_loader_handle;
typedef void (*amdb_loader_load_function)(uint64_t task_info);

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

amdb_parallel_loader *amdb_loader_create(void);
amdb_loader_handle *amdb_loader_get_handle(amdb_parallel_loader *obj_ptr, adsp_amdb_get_modules_callback_f callback_f, void *context_ptr, amdb_loader_load_function load_function);
void amdb_loader_push_task(amdb_loader_handle *loader_handle_ptr, uint64_t task_info);
void amdb_loader_release_handle(amdb_loader_handle *loader_handle_ptr);
void amdb_loader_destroy(amdb_parallel_loader *obj_ptr);

#endif // #ifndef ADSP_AMDB_PARALLEL_LOADER_H
