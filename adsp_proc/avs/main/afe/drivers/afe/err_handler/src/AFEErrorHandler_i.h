/*========================================================================
   This file contains AFE Error handler implementation

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: 
 ====================================================================== */

#ifndef _AFE_ERR_HNDLR_I_H_
#define _AFE_ERR_HNDLR_I_H_

/*==========================================================================
  Include files
  ========================================================================== */

#include "qurt_elite.h"

/*----------------------------------------------------------------------------
 * Macro definition
 * -------------------------------------------------------------------------*/

/**<For production build, perform always device recovery.*/

/**<Within 1ms window interval, never three signal misses can take place
 * Hence, will always allows device recoveries.*/

#define MAX_NUM_ALLOWED_RECOVERIES                2
/**< Indicates maximum number of recoveries allowed in a certain window duration.*/
#define ERR_HNDLR_REC_WINDOW_DURATION          1000     //ex:1000 micro seconds
/**< Indicates the window duration in micro seconds.
 *  Used as reference to crash/allow recovery depends on number of recoveries taken place.*/

typedef struct afe_device_recovery_params
{
  uint32_t   max_num_allowed_recoveries;
  /**< Indicates maximum number of recoveries
   * allowed in a certain window duration.*/
  uint32_t   window_duration;
  /**< Indicates the window duration in micro seconds.*/

  /* These parameters collectively decides
   * whether to crash or allow device recovery.*/

}afe_device_recovery_params_t;

/**< Pointer to access global device recovery params structure .*/
extern afe_device_recovery_params_t *dev_rec_params_global_ptr;

typedef struct afe_err_handler_info
{
  uint64_t *ts_queue_ptr;
  /**< Queue that holds time stamp of past device
   * recoveries. ts_queue will be allocated dynamically
   * depends on number of allowed recoveries.
   * */
  uint16_t front_idx;
  /**< Indicates front index of the queue.
   * Front index will always points to first(oldest)
   * device recovery time stamp in current window.*/
  uint16_t back_idx;
  /**< Indicates back index of the queue
   * Back index will always points to recent(latest)
   * device recovery time stamp in current window.*/
  uint16_t err_recovery_count;
  /**< error recovery count indicates number of
   * recoveries taken place in current window.*/
} afe_err_handler_info_t;

/** Error handling types */
typedef enum afe_err_hndlr_type_t
{
  AFE_ERR_HNDL_NONE = 0,  /**< No Error Handling */
  AFE_DEBUG_CRASH,        /**< Force crash for worker thread signal miss */
  AFE_ERR_RECOVERY        /**< Recover device from permanent mute or stall due to signal miss */
} afe_err_hndlr_type_t;


/*==========================================================================
  Function declarations
  ========================================================================== */

afe_err_hndlr_type_t afe_get_err_handler_type(uint32_t port_id);


#endif /* _AFE_ERR_HNDLR_I_H_ */
