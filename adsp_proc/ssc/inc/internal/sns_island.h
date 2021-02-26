#pragma once
/*============================================================================
  @file sns_island.h

  @brief
  Island control for Sensors.

  Copyright (c) 2016 - 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================*/

/*============================================================================
  INCLUDES
  ============================================================================*/
#include <stdint.h>
#include "sns_osa_thread.h"
#include "sns_rc.h"

#include "sns_printf_int.h"

#include "sns_island_util.h"
#include "sns_sensor_uid.h"

/*============================================================================
  MACROS
  ============================================================================*/

/*!
  @brief Exit island mode.

  @note The system will enter island mode at a later time after current thread
    has returned to IDLE
*/

#define SNS_ISLAND_EXIT()  \
{  \
  if(SNS_RC_SUCCESS == sns_island_exit_internal())  \
  {  \
    SNS_PRINTF(MED, sns_fw_printf, "Island exit");  \
  }  \
} (void)0

/*!
 *  SNS_SECTION( name )
 *
 *  This macro can be used to place code and variables into a section called name.
 *
 *  To move code or variables into island mode on supported targets, first tag the
 *  item using this macro, next use the same sections in addIslandLibrary() method
 *  in the scons file to inform the linker that a section must be made available
 *  in island memory.
 */

#define SNS_SECTION(name)  \
  __attribute__((section(name)))

/*============================================================================
  Functions
  ============================================================================*/
/*!
  @brief Internal call to exit island mode.

  @note Do not call this function directly. Use the SNS_ISLAND_EXIT() macro instead

  @return
  SNS_RC_SUCCESS              island exit was successful
  SNS_RC_NOT_AVAILABLE        island exit transition did not happen.
                              This could be due to:
                              - island mode not supported on target/disabled
                              - system is not in island mode when exit was requested
*/
sns_rc sns_island_exit_internal( void );

/*!
  @brief Generates a log packet of the sns_island_trace_log type and commits it.

  @param[i] user_defined_id   The value to be added to the cookie field in
                              the log packet

  @return
  SNS_RC_SUCCESS              log packet operation was successful
  SNS_RC_NOT_SUPPORTED        log packet not supported
*/
sns_rc sns_island_log_trace(
  uint64_t user_defined_id );

/**
 * @brief Initialize the island service
 *
 * @return The island service
 */
sns_service* sns_island_service_init(void);

