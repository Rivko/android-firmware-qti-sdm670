/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/utils/src/algo_logging_i.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/22/2014   rv       Created

==========================================================================*/

/*-----------------------------------------------------------------------
   This file contains Algorithm logging related apis.

   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/

#ifndef _ALGO_LOGGING_I_H_
#define _ALGO_LOGGING_I_H_

/*==========================================================================
  Include files
  ========================================================================== */
#include "Elite.h"
#include "log.h"
#include "diagpkt.h"
#include "EliteLoggingUtils_i.h"
#include "algo_logging.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
	  Macro Defines
========================================================================== */

/* Maximum logging interval supported in millisec, used to determine the size of the log buffer */
#define ALGO_LOG_INTERVAL_IN_MS_MAX	60


/** @brief Node for storing the linked list of clients. */
typedef struct client_list_t
{
   void *element;
   /**< This element. */
   struct client_list_t *next;
   /**< Pointer to the next node. */
} client_list_t;


struct algo_data_logging_t
{
   uint16_t client_counter;
   /**< counter to track the number of clients */
   void     *client_list_ptr;
   /**< log client linked list ptr */
};


typedef struct algo_log_client_info
{
   uint32_t client_handle;
   /**< client handle */
   uint16_t log_interval_in_ms;
   /**< logging interval in ms */
   uint32_t log_size;
   /**< log size in bytes */
   int8_t   *log_buf_ptr;
   /**< buf ptr */
   uint32_t log_size_used;
   /**< used size, bytes */
   uint32_t log_buf_size;
   /**< log buf size in bytes per diag commit. */
   elite_qxdm_log_code log_code;
   /**< QXDM log code */
} algo_log_client_info_t;


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _ALGO_LOGGING_I_H_ */
