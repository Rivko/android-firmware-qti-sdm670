#ifndef PD_DUMP_H
#define PD_DUMP_H
/** vi: tw=128 ts=3 sw=3 et
@file pd_dump.h
@brief This file contains the API details for the Protection Domain Monitor, API 1.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 - 2019 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/api/debugtools/pd_dump.h#1 $
$DateTime: 2019/02/18 01:48:20 $
$Change: 18354452 $
$Author: pwbldsvc $
===============================================================================*/
#include "comdef.h"

#include "qurt.h"
#include "qurt_qdi_driver.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* Error codes while registering or de-registering PD dump callbacks */
typedef enum{
  PDDUMP_E_SUCCESS = 0,
  PDDUMP_E_FAILURE = -1,
  PDDUMP_E_INVALID_PARAM = -2,
  PDDUMP_E_NOT_ALLOWED = -3,
}pd_dump_cb_error_t;

/* PD dump callback type */
/* client_handle of the process, can be used to get process info */
/* data1 is a pointer to a list of process Thread IDs (qurt_thread_t) */
/* data2 is the number of entries pointed to by data1 */
/* user_data is user specific context or user data if any */
typedef void (*pd_dump_cb_fn_ptr)(int client_handle, uint32 data1, uint32 data2, uint32 user_data);

/* PD dump callbacks information need to provide via this struct. */
typedef struct{
  pd_dump_cb_fn_ptr cb_fn;
  uint32 user_data;
}pd_dump_cb_info_t;

/** pd_dump_cb_enable
 * This will allow clients to register for callback to be invoke in path
 * of PD dump handling post-crash.
 *
 * @param cb_info pd_dump_cb_info_t structure having required info.
 *
 * @return PDDUMP_E_SUCCESS in case of successful registration else
 *         pd_dump_cb_error_t error codes.
 */
pd_dump_cb_error_t pd_dump_cb_enable(pd_dump_cb_info_t * cb_info);

/** pd_dump_cb_disable
 * This will allow clients to de-register callback, which was register via 
 * pd_dump_cb_enable.
 *
 * @param cb_info pd_dump_cb_info_t structure having required info.
 *
 * @return PDDUMP_E_SUCCESS in case of successful de-registration else
 *         pd_dump_cb_error_t error codes.
 */
pd_dump_cb_error_t pd_dump_cb_disable(pd_dump_cb_info_t * cb_info);


#if defined(__cplusplus)
}
#endif

#endif
