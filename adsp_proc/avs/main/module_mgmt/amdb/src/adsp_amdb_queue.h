/*========================================================================
ADSP AMDB Parallel Queue

This file describes the interface for the queue used by the AMDB parallel loader.

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

#ifndef ADSP_AMDB_QUEUE_H
#define ADSP_AMDB_QUEUE_H


/*----------------------------------------------------------------------------
 * Include files
 * -------------------------------------------------------------------------*/
#ifndef ADSP_AMDB_TEST
#include "mmdefs.h"
#include "adsp_error_codes.h"
#else
#include "test_main.h"
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

struct adsp_amdb_queue_t;

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

adsp_amdb_queue_t *amdb_queue_create(uint32_t max_items, uint32_t size_of_element, char name[]);
void amdb_queue_push(adsp_amdb_queue_t *queue_ptr, const void *in_data_ptr);
void amdb_queue_pop(adsp_amdb_queue_t *queue_ptr, void *out_data_ptr);
void amdb_queue_destroy(adsp_amdb_queue_t *queue_ptr);

#endif // #ifndef ADSP_AMDB_QUEUE_H
